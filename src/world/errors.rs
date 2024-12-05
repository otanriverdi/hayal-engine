use thiserror::Error;

#[derive(Error, Debug)]
pub enum WorldError {
    #[error("component was never spawned")]
    ComponentNotFound,
    #[error("entity not found")]
    EntityNotFound,
    #[error("component not slotted to entry")]
    ComponentNotSlotted,
}
