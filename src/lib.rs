use tracing::{instrument, warn};
use winit::application::ApplicationHandler;
use winit::event::WindowEvent;
use winit::event_loop::ActiveEventLoop;
use winit::event_loop::{ControlFlow, EventLoop};
use winit::window::{Window as WinitWindow, WindowId};

use crate::scheduler::Scheduler;
pub use crate::scheduler::{Schedule, System};
pub use crate::world::World;

mod renderer;
mod scheduler;
mod world;

pub struct Engine {
    window: Option<WinitWindow>,
    world: World,
    scheduler: Scheduler,
}

// TODO: abstract window
impl Engine {
    pub fn init() -> Self {
        let world = World::default();
        let scheduler = Scheduler::new();
        Self {
            window: None,
            world,
            scheduler,
        }
    }

    pub fn run(&mut self) {
        let event_loop = EventLoop::new().unwrap();
        event_loop.set_control_flow(ControlFlow::Poll);
        event_loop.run_app(self).unwrap();
    }

    pub fn with_system(&mut self, schedule: Schedule, system: System) -> &mut Self {
        self.scheduler.add_system(schedule, system);
        self
    }

    fn on_init(&self) {
        self.scheduler.run_schedule(Schedule::Init, &self.world);
    }

    fn on_update(&self, _event: WindowEvent) {
        self.scheduler
            .run_schedule(Schedule::Update, &self.world);
    }

    fn on_deinit(&self) {
        self.scheduler
            .run_schedule(Schedule::Deinit, &self.world);
    }
}

impl ApplicationHandler for Engine {
    #[instrument(skip_all, name = "window_init")]
    fn resumed(&mut self, event_loop: &ActiveEventLoop) {
        self.window = Some(
            event_loop
                .create_window(WinitWindow::default_attributes())
                .unwrap(),
        );
        self.on_init();
    }

    fn window_event(&mut self, event_loop: &ActiveEventLoop, _id: WindowId, event: WindowEvent) {
        if event == WindowEvent::CloseRequested {
            self.on_deinit();
            event_loop.exit();
        }
        self.on_update(event);
    }

    fn about_to_wait(&mut self, _event_loop: &ActiveEventLoop) {
        self.window.as_ref().unwrap().request_redraw();
    }
}
