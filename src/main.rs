use hayal::{builtins::{Transform, Text}, Engine, World};
use tracing_subscriber::fmt::format::FmtSpan;

fn hello_world_system(world: &World) {
    let entity = world.spawn_entity(Transform { x: 12, y: 12 });
    world.add_component(entity, Text { text: "Hello World!", font_size: 70}).unwrap();
}

fn main() {
    tracing_subscriber::fmt()
        .with_span_events(FmtSpan::CLOSE)
        .init();

    Engine::init().with_system(hayal::Schedule::Init, hello_world_system).run();
}
