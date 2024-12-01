use hayal::window::Window;

fn main() {
    tracing_subscriber::fmt::init();

    let mut window = Window::default();
    window.run().unwrap();
}
