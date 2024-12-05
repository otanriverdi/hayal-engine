use hayal::engine::{Engine, Schedule, World};
use tracing::info;
use tracing_subscriber::fmt::format::FmtSpan;

fn test_system(_world: &mut World) {
    info!("system 1")
}

fn test_system_2(_world: &mut World) {
    info!("system 2")
}

fn main() {
    tracing_subscriber::fmt()
        .with_span_events(FmtSpan::CLOSE)
        .init();

    Engine::init()
        .with_system(Schedule::Update, test_system)
        .with_system(Schedule::Update, test_system_2)
        .run();
}
