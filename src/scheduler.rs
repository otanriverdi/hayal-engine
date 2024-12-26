use std::collections::HashMap;

use raylib::prelude::RaylibDrawHandle;

use crate::world::World;

#[derive(Debug, PartialEq, Eq, Hash)]
pub enum Schedule {
    Init,
    Update,
    Deinit,
}

pub type System = fn(&World);
pub type RenderSystem = fn(&World, &mut RaylibDrawHandle);
type SystemVector = Vec<System>;
type RenderSystemVector = Vec<RenderSystem>;

pub struct Scheduler {
    systems: HashMap<Schedule, SystemVector>,
    render_systems: RenderSystemVector,
}

impl Scheduler {
    pub fn new() -> Self {
        Self {
            systems: HashMap::new(),
            render_systems: Vec::new(),
        }
    }

    pub fn add_system(&mut self, schedule: Schedule, system: System) {
        self.systems.entry(schedule).or_default().push(system);
    }

    pub fn add_render_system(&mut self, system: RenderSystem) {
        self.render_systems.push(system);
    }

    pub fn run_schedule(&self, schedule: Schedule, world: &World) {
        if let Some(systems) = self.systems.get(&schedule) {
            for system in systems {
                system(world)
            }
        }
    }

    pub fn run_render_schedule(
        &self,
        world: &World,
        renderer: &mut RaylibDrawHandle,
    ) {
            for system in self.render_systems.iter() {
                system(world, renderer)
            }
    }
}
