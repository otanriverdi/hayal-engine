use std::collections::HashMap;

use crate::world::World;

#[derive(Debug, PartialEq, Eq, Hash)]
pub enum Schedule {
    Init,
    Update,
    Deinit,
}

pub type System = fn(&mut World);
type SystemVector = Vec<System>;


pub struct Scheduler {
    systems: HashMap<Schedule, SystemVector>
}

impl Scheduler {
    pub fn new() -> Self {
        Self {
            systems: HashMap::new(),
        }
    }

    pub fn add_system(&mut self, schedule: Schedule, system: System) {
        // TODO: we need the ability to pass in a tuple of functions that will be 
        // guaranteed to execute one after another
        self.systems.entry(schedule).or_default().push(system);
    }

    pub fn run_schedule(&self, schedule: Schedule, world: &mut World) {
        if let Some(systems) = self.systems.get(&schedule) {
            for system in systems {
                system(world)
            }
        }
    }
}
