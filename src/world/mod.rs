use std::{
    any::{Any, TypeId},
    collections::HashMap,
    sync::{Arc, Mutex},
};

use errors::WorldError;
use query::QueryBuilder;
use tracing::instrument;

mod errors;
mod query;

type Component = Arc<Mutex<dyn Any>>;
type SparseSet = Vec<Option<Component>>;
type Storage = HashMap<TypeId, SparseSet>;

#[derive(Default)]
pub struct World {
    resources: Arc<Mutex<HashMap<TypeId, Arc<dyn Any>>>>,
    sparse_sets: Arc<Mutex<Storage>>,
    bit_masks: Arc<Mutex<HashMap<TypeId, u128>>>,
    entity_maps: Arc<Mutex<Vec<u128>>>,
}

impl World {
    #[instrument(skip_all, fields(resource = %std::any::type_name_of_val(&initial)))]
    pub fn register_resource(&self, initial: impl Any) {
        let type_id = initial.type_id();
        self.resources.lock().unwrap().insert(type_id, Arc::new(initial));
    }

    pub fn get_resource<T: Any>(&self) -> Option<Arc<T>> {
        let type_id = TypeId::of::<T>();
        let resources = self.resources.lock().unwrap();
        if let Some(resource) = resources.get(&type_id) {
            return resource.downcast_ref::<Arc<T>>().cloned();
        }
        None
    }

    #[instrument(skip_all, fields(T = %std::any::type_name::<T>()))]
    pub fn delete_resource<T: Any>(&self) {
        let type_id = TypeId::of::<T>();
        self.resources.lock().unwrap().remove(&type_id);
    }

    #[instrument(skip_all)]
    pub fn spawn_entity<T: IntoIterator<Item = impl Any>>(&self, components: T) -> usize {
        let owned_map = self.entity_maps.clone();
        let mut entity_maps = owned_map.lock().unwrap();
        let mut entity_id = entity_maps.len() - 1;
        if let Some((idx, _)) = entity_maps.iter().enumerate().find(|(_, mask)| **mask == 0) {
            entity_id = idx
        } else {
            self.sparse_sets
                .lock()
                .unwrap()
                .iter_mut()
                .for_each(|(_type_id, set)| set.push(None));
            entity_maps.push(0);
        }
        components.into_iter().for_each(|component| {
            self.add_component(entity_id, component)
                .expect("Entity not initialized properly")
        });
        entity_id
    }

    pub fn query(&self) -> QueryBuilder {
        QueryBuilder::new(self)
    }

    #[instrument(skip(self))]
    pub fn delete_entity(&self, entity_id: usize) {
        self.entity_maps.lock().unwrap()[entity_id] = 0;
    }

    #[instrument(skip(self, component), fields(component = %std::any::type_name_of_val(&component)))]
    pub fn add_component(
        &self,
        entity_id: usize,
        component: impl Any,
    ) -> Result<(), WorldError> {
        let type_id = component.type_id();
        self.ensure_component(type_id);
        let mut sparse_sets = self.sparse_sets.lock().unwrap();
        let components = sparse_sets
            .get_mut(&type_id)
            .expect("Missing sparse set for ensured component");
        components[entity_id] = Some(Arc::new(Mutex::new(component)));
        let bit_masks = self.bit_masks.lock().expect("Failed to lock bit masks");
        let bitmask = bit_masks
            .get(&type_id)
            .expect("Missing bitmask for ensured component");
        let mut entity_maps = self.entity_maps.lock().unwrap();
        let entity_map = entity_maps
            .get_mut(entity_id)
            .ok_or(WorldError::EntityNotFound)?;
        *entity_map |= *bitmask;
        Ok(())
    }

    #[instrument(skip(self), fields(T = %std::any::type_name::<T>()))]
    pub fn delete_component<T: Any>(&self, entity_id: usize) {
        let type_id = TypeId::of::<T>();
        let bit_masks = self.bit_masks.lock().unwrap();
        if let Some(bit_mask) = bit_masks.get(&type_id) {
            if self.has_bit_mask(entity_id, *bit_mask) {
                let mut entity_maps = self.entity_maps.lock().unwrap();
                entity_maps[entity_id] ^= *bit_mask;
            }
        }
    }

    pub fn get_component<T: Any>(&self, entity_id: usize) -> Result<Component, WorldError> {
        let type_id = TypeId::of::<T>();
        let sparse_sets = self.sparse_sets.lock().unwrap();
        let component = sparse_sets
            .get(&type_id)
            .ok_or(WorldError::ComponentNotFound)?[entity_id]
            .as_ref()
            .ok_or(WorldError::ComponentNotSlotted)?;
        Ok(component.clone())
    }

    fn has_bit_mask(&self, entity_id: usize, mask: u128) -> bool {
        self.entity_maps.lock().unwrap()[entity_id] & mask == mask
    }

    #[instrument(skip_all, fields(query = %query.bit_map))]
    fn run_query(&self, query: &QueryBuilder) -> Vec<usize> {
        self.entity_maps
            .lock()
            .unwrap()
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

    fn ensure_component(&self, type_id: TypeId) {
        let mut sparse_sets = self.sparse_sets.lock().unwrap();
        if !sparse_sets.contains_key(&type_id) {
            let entity_count = sparse_sets
                .values()
                .next()
                .map(|vec| vec.len())
                .unwrap_or(0);

            sparse_sets.insert(type_id, vec![None; entity_count]);
            let mut bit_masks = self.bit_masks.lock().unwrap();
            let len = bit_masks.len();
            bit_masks.insert(type_id, 1 << len);
        }
    }
}
