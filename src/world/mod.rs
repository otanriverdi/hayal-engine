use std::{
    any::{Any, TypeId},
    cell::{Ref, RefCell, RefMut},
    collections::HashMap,
    rc::Rc,
};

use errors::WorldError;
use query::QueryBuilder;
use tracing::instrument;

mod errors;
mod query;

type ComponentData = Rc<RefCell<dyn Any>>;
type SparseSet = Vec<Option<ComponentData>>;
type Storage = HashMap<TypeId, SparseSet>;

#[derive(Default)]
pub struct World {
    resources: HashMap<TypeId, Box<dyn Any>>,
    sparse_sets: Storage,
    bit_masks: HashMap<TypeId, u128>,
    entity_maps: Vec<u128>,
}

impl World {
    #[instrument]
    pub fn new() -> Self {
        Self::default()
    }

    #[instrument(skip_all, fields(resource = %std::any::type_name_of_val(&initial)))]
    pub fn register_resource(&mut self, initial: impl Any) {
        let type_id = initial.type_id();
        self.resources.insert(type_id, Box::new(initial));
    }

    pub fn get_resource_ref<T: Any>(&self) -> Option<&T> {
        let type_id = TypeId::of::<T>();
        if let Some(resource) = self.resources.get(&type_id) {
            return resource.downcast_ref::<T>();
        }
        None
    }

    pub fn get_resource_ref_mut<T: Any>(&mut self) -> Option<&mut T> {
        let type_id = TypeId::of::<T>();
        if let Some(resource) = self.resources.get_mut(&type_id) {
            return resource.downcast_mut::<T>();
        }
        None
    }

    #[instrument(skip_all, fields(T = %std::any::type_name::<T>()))]
    pub fn delete_resource<T: Any>(&mut self) {
        let type_id = TypeId::of::<T>();
        self.resources.remove(&type_id);
    }

    #[instrument(skip_all)]
    pub fn spawn_entity<T: IntoIterator<Item = impl Any>>(&mut self, components: T) -> usize {
        let mut entity_id = self.entity_maps.len() - 1;
        if let Some((idx, _)) = self
            .entity_maps
            .iter()
            .enumerate()
            .find(|(_, mask)| **mask == 0)
        {
            entity_id = idx
        } else {
            self.sparse_sets
                .iter_mut()
                .for_each(|(_type_id, set)| set.push(None));
            self.entity_maps.push(0);
        }
        components
            .into_iter()
            .for_each(|component| self.add_component(entity_id, component).unwrap());
        entity_id
    }

    pub fn query(&self) -> QueryBuilder {
        QueryBuilder::new(self)
    }

    #[instrument(skip(self))]
    pub fn delete_entity(&mut self, entity_id: usize) {
        self.entity_maps[entity_id] = 0;
    }

    #[instrument(skip(self, component), fields(component = %std::any::type_name_of_val(&component)))]
    pub fn add_component(
        &mut self,
        entity_id: usize,
        component: impl Any,
    ) -> Result<(), WorldError> {
        let type_id = component.type_id();
        self.ensure_component(type_id);
        let components = self.sparse_sets.get_mut(&type_id).unwrap();
        components[entity_id] = Some(Rc::new(RefCell::new(component)));
        let bitmask = self.bit_masks.get(&type_id).unwrap();
        let entity_map = self
            .entity_maps
            .get_mut(entity_id)
            .ok_or(WorldError::EntityNotFound)?;
        *entity_map |= *bitmask;
        Ok(())
    }

    #[instrument(skip(self), fields(T = %std::any::type_name::<T>()))]
    pub fn delete_component<T: Any>(&mut self, entity_id: usize) {
        let type_id = TypeId::of::<T>();
        if let Some(bit_mask) = self.bit_masks.get(&type_id) {
            if self.has_bit_mask(entity_id, *bit_mask) {
                self.entity_maps[entity_id] ^= *bit_mask;
            }
        }
    }

    pub fn get_component<T: Any>(&self, entity_id: usize) -> Result<Ref<T>, WorldError> {
        let borrowed = self.get_component_ref::<T>(entity_id)?.borrow();
        Ok(Ref::map(borrowed, |any| any.downcast_ref::<T>().unwrap()))
    }

    pub fn get_component_mut<T: Any>(&self, entity_id: usize) -> Result<RefMut<T>, WorldError> {
        let borrowed = self.get_component_ref::<T>(entity_id)?.borrow_mut();
        Ok(RefMut::map(borrowed, |any| {
            any.downcast_mut::<T>().unwrap()
        }))
    }

    fn get_component_ref<T: Any>(&self, entity_id: usize) -> Result<&ComponentData, WorldError> {
        let type_id = TypeId::of::<T>();
        self.sparse_sets
            .get(&type_id)
            .ok_or(WorldError::ComponentNotFound)?[entity_id]
            .as_ref()
            .ok_or(WorldError::ComponentNotSlotted)
    }

    fn has_bit_mask(&self, entity_id: usize, mask: u128) -> bool {
        self.entity_maps[entity_id] & mask == mask
    }

    #[instrument(skip_all, fields(query = %query.bit_map))]
    fn run_query(&self, query: &QueryBuilder) -> Vec<usize> {
        self.entity_maps
            .iter()
            .enumerate()
            .filter_map(|(idx, entity_map)| {
                if entity_map & query.bit_map == query.bit_map {
                    return Some(idx);
                }
                None
            })
            .collect()
    }

    fn ensure_component(&mut self, type_id: TypeId) {
        if !self.sparse_sets.contains_key(&type_id) {
            let entity_count = self
                .sparse_sets
                .values()
                .next()
                .map(|vec| vec.len())
                .unwrap_or(0);

            self.sparse_sets.insert(type_id, vec![None; entity_count]);
            self.bit_masks.insert(type_id, 1 << self.bit_masks.len());
        }
    }
}
