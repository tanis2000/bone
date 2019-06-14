extern crate clap;
#[macro_use]
extern crate derive_builder;
extern crate handlebars;
#[macro_use]
extern crate serde;
#[macro_use]
extern crate serde_json;

mod cmd;
mod utils;

use clap::{Arg, App, SubCommand};
use cmd::init::{InitBuilder};
use cmd::update::{UpdateBuilder};
use cmd::upgrade::{UpgradeBuilder};
use cmd::build::{BuildBuilder};
use cmd::run::{RunBuilder};

fn main() {
    let mut default_target_os = "unknown";
    if cfg!(target_os = "macos") {
        default_target_os = "mac";
    } else if cfg!(target_os = "windows") {
        default_target_os = "windows";
    } else if cfg!(target_os = "linux") {
        default_target_os = "linux";
    } else if cfg!(target_os = "android") {
        default_target_os = "android";
    } else if cfg!(target_os = "ios") {
        default_target_os = "ios";
    } else if cfg!(target_os = "wasm") {
        default_target_os = "web";
    }

    let matches = App::new("Bone")
    .version("0.1.0")
    .author("Valerio Santinelli")
    .subcommand(SubCommand::with_name("init")
    .about("Initialize a new Binocle project")
    .arg(Arg::with_name("project_name")
    .help("The name of the project")
    .index(1)
    .required(true))
    .arg(Arg::with_name("dir")
    .help("The directory to create the project into")
    .index(2)
    .required(true))
    .arg(Arg::with_name("engine")
    .help("The engine to use: binocle or binocle-c")
    .short("e")
    .long("engine")
    .takes_value(true))
    )
    .subcommand(SubCommand::with_name("update")
    .about("Update a Binocle project. Only the CMake files will be overwritten. Use with caution and make a backup of your project.")
    .arg(Arg::with_name("dir")
    .help("The directory of the project to update")
    .index(1)
    .required(true))
    )
    .subcommand(SubCommand::with_name("upgrade")
    .about("Upgrade the version of Binocle in an existing project.")
    .arg(Arg::with_name("dir")
    .help("The directory of the project to upgrade")
    .index(1)
    .required(true))
    )
    .subcommand(SubCommand::with_name("build")
    .about("Build a Binocle project")
    .arg(Arg::with_name("dir")
    .help("The directory of the project to upgrade")
    .index(1)
    .required(true))
    .arg(Arg::with_name("target")
    .help("The target platform")
    .index(2)
    .default_value(default_target_os))
    )
    .subcommand(SubCommand::with_name("run")
    .about("Run a Binocle project")
    .arg(Arg::with_name("dir")
    .help("The directory of the project to run")
    .index(1)
    .required(true))
    .arg(Arg::with_name("target")
    .help("The target platform")
    .index(2)
    .default_value(default_target_os))
    )
    .get_matches();

    

    match matches.subcommand_name() {
        Some("init") => {
            let matches = matches.subcommand_matches("init").unwrap();
            let mut engine: Option<String> = None;
            match matches.value_of("engine") {
                Some(e) => {
                    engine = Some(e.to_string());
                },
                None => {},
            }
            let cmd = InitBuilder::default()
            .app(String::new())
            .project_name(Some(matches.value_of("project_name").unwrap().to_string()))
            .dir(Some(matches.value_of("dir").unwrap().to_string()))
            .engine(engine).build();
            match cmd {
                Ok(cmd) => {
                    cmd.run();
                },
                Err(e) => {
                    println!("{}", e);
                }
            }
        },
        Some("update") => {
            let matches = matches.subcommand_matches("update").unwrap();
            let cmd = UpdateBuilder::default()
            .app(String::new())
            .dir(Some(matches.value_of("dir").unwrap().to_string()))
            .build();
            match cmd {
                Ok(cmd) => {
                    cmd.run();
                },
                Err(e) => {
                    println!("{}", e);
                }
            }
        },
        Some("upgrade") => {
            let matches = matches.subcommand_matches("upgrade").unwrap();
            let cmd = UpgradeBuilder::default()
            .app(String::new())
            .dir(Some(matches.value_of("dir").unwrap().to_string()))
            .build();
            match cmd {
                Ok(cmd) => {
                    cmd.run();
                },
                Err(e) => {
                    println!("{}", e);
                }
            }
        },
        Some("build") => {
            let matches = matches.subcommand_matches("build").unwrap();
            let cmd = BuildBuilder::default()
            .app(String::new())
            .dir(Some(matches.value_of("dir").unwrap().to_string()))
            .target(Some(matches.value_of("target").unwrap().to_string()))
            .build();
            match cmd {
                Ok(cmd) => {
                    cmd.run();
                },
                Err(e) => {
                    println!("{}", e);
                }
            }
        },
        Some("run") => {
            let matches = matches.subcommand_matches("run").unwrap();
            let cmd = RunBuilder::default()
            .app(String::new())
            .dir(Some(matches.value_of("dir").unwrap().to_string()))
            .target(Some(matches.value_of("target").unwrap().to_string()))
            .build();
            match cmd {
                Ok(cmd) => {
                    cmd.run();
                },
                Err(e) => {
                    println!("{}", e);
                }
            }
        },
        None => {},
        _ => {},
    }
}