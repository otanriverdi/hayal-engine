use hayal::window::Window;
use tracing_subscriber::fmt::format::FmtSpan;
use winit::event_loop::{ControlFlow, EventLoop};

fn main() {
    tracing_subscriber::fmt()
        .with_span_events(FmtSpan::CLOSE)
        .init();
    let event_loop = EventLoop::new().unwrap();
    event_loop.set_control_flow(ControlFlow::Poll);
    let mut window = Window::default();
    event_loop.run_app(&mut window).unwrap();
}
