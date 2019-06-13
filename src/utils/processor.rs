use handlebars::Handlebars;
use std::fs::File;
use std::io;
use std::io::prelude::*;
use std::path::Path;

pub struct Processor {

}

impl Processor {
    pub fn process(bytes: &[u8], path: &Path, context: &serde_json::Value) {
        let mut reg = Handlebars::new();
        let rendered = reg.render_template(String::from_utf8(bytes.to_vec()).unwrap().as_str(), context).unwrap();
        let mut file = File::create(path);
        match file {
            Ok(mut file) => {
                file.write_all(rendered.as_bytes());
            },
            Err(e) => {
                println!("Error: cannot create file {} {}", path.to_string_lossy(), e.to_string());
            }
        }

    }
}