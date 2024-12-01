use std::any::{Any, TypeId};

use super::World;

pub struct QueryBuilder<'a> {
    world: &'a World,
    pub bit_map: u128,
}

// TODO: implement proper error handling
impl<'a> QueryBuilder<'a> {
    pub fn new(world: &'a World) -> Self {
        Self { world, bit_map: 0 }
    }

    pub fn with_component<T: Any>(&mut self) -> &mut Self {
        let type_id = TypeId::of::<T>();
        let bit_mask = self.world.bit_masks.get(&type_id).unwrap();
        self.bit_map |= *bit_mask;
        self
    }

    pub fn run(&self) -> Vec<usize> {
        self.world.run_query(self)
    }
}

