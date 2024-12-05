use tracing::{instrument, warn};
use winit::application::ApplicationHandler;
use winit::event::WindowEvent;
use winit::event_loop::ActiveEventLoop;
use winit::event_loop::{ControlFlow, EventLoop};
use winit::window::{Window as WinitWindow, WindowId};

pub use crate::schedule::{Schedule, System};
use crate::schedule::Scheduler;
pub use crate::world::World;

pub struct Engine {
    window: Option<WinitWindow>,
    world: World,
    scheduler: Scheduler,
}

impl Engine {
    pub fn init() -> Self {
        let world = World::new();
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

    fn on_init(&mut self) {
        self.scheduler.run_schedule(Schedule::Init, &mut self.world);
    }

    fn on_update(&mut self, _event: WindowEvent) {
        // TODO: this is where we update builtin resources like inputs, events etc.
        self.scheduler.run_schedule(Schedule::Update, &mut self.world);
        // TODO: this is where we render builtin components with meshes
    }

    fn on_deinit(&mut self) {
        self.scheduler.run_schedule(Schedule::Deinit, &mut self.world);
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
