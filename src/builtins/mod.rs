use std::sync::Arc;

use raylib::{
    color::Color,
    prelude::{RaylibDraw, RaylibDrawHandle},
};

use crate::World;

pub struct BackgroundColor {
    color: Color,
}

pub fn set_background_color_system(world: &World, renderer: &mut RaylibDrawHandle) {
    let bg_color = world
        .get_resource::<BackgroundColor>()
        .unwrap_or(Arc::new(BackgroundColor { color: Color::BLUE }));
    renderer.clear_background(bg_color.color);
}

#[derive(Debug, Clone)]
pub struct Transform {
    pub x: i32,
    pub y: i32,
}

#[derive(Debug, Clone)]
pub struct Text {
    pub text: &'static str,
    pub font_size: i32,
}

pub fn draw_text_system(world: &World, renderer: &mut RaylibDrawHandle) {
    for entity in world
        .query()
        .with_component::<Text>()
        .with_component::<Transform>()
        .run()
        .into_iter()
    {
        let cmp = world.get_component::<Transform>(entity).unwrap();
        let locked = cmp.lock().unwrap();
        let transform = locked.downcast_ref::<Transform>().unwrap();

        let cmp = world.get_component::<Text>(entity).unwrap();
        let locked = cmp.lock().unwrap();
        let text = locked.downcast_ref::<Text>().unwrap();
        renderer.draw_text(
            text.text,
            transform.x,
            transform.y,
            text.font_size,
            Color::WHITE,
        );
    }
}
