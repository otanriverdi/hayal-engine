use builtins::{draw_text_system, set_background_color_system};
use raylib::{RaylibHandle, RaylibThread};

pub use crate::scheduler::Schedule;
use crate::scheduler::{Scheduler, System};
pub use crate::world::World;

pub mod builtins;
mod scheduler;
mod world;

pub struct Engine {
    world: World,
    scheduler: Scheduler,
    rl: RaylibHandle,
    rl_thread: RaylibThread,
}

impl Engine {
    pub fn init() -> Self {
        let world = World::default();
        let scheduler = Scheduler::new();
        let (rl, rl_thread) = raylib::init().size(640, 480).build();
        Self {
            world,
            scheduler,
            rl,
            rl_thread,
        }
    }

    pub fn run(&mut self) {
        self.add_builtin_systems();
        self.scheduler.run_schedule(Schedule::Init, &self.world);

        while !self.rl.window_should_close() {
            let mut renderer = self.rl.begin_drawing(&self.rl_thread);
            self.scheduler.run_schedule(Schedule::Update, &self.world);
            self.scheduler
                .run_render_schedule(&self.world, &mut renderer);
        }

        self.scheduler.run_schedule(Schedule::Deinit, &self.world);
    }

    pub fn with_system(&mut self, schedule: Schedule, system: System) -> &mut Self {
        self.scheduler.add_system(schedule, system);
        self
    }

    fn add_builtin_systems(&mut self) {
        self.scheduler.add_render_system(draw_text_system);
        self.scheduler
            .add_render_system(set_background_color_system);
    }
}
