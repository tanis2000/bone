use serde::{Deserialize, Serialize};
use serde_json::{Result, Value};
use std::collections::HashMap;
use std::io;
use std::io::prelude::*;
use std::fs::{self, DirEntry, File};
use std::path::Path;
use std::process::{Command, Stdio};
use std::time::Duration;
use crate::utils::Processor;
use crate::utils::Target;

#[derive(Serialize, Deserialize)]
struct Device {
    name: String,
    udid: String,
}

#[derive(Serialize, Deserialize)]
struct Simulators {
    devices: HashMap<String, Vec<Device>>
}

#[derive(Default, Builder, Debug)]
#[builder(setter(into))]
pub struct Run {
    app: Option<String>,
    dir: Option<String>,
    target: Option<String>,
}

impl Run {
    pub fn run(&self) {
        let app = self.app.clone().unwrap();
        let dir = self.dir.clone().unwrap();
        let target_ss = self.target.clone().unwrap().clone();
        let target_s = target_ss.as_str();
        let mut target = Target::Unknown;

        match target_s {
            "mac" => {
                target = Target::Mac
            },
            "windows" => {
                target = Target::Windows
            },
            "linux" => {
                target = Target::Linux
            },
            "ios" => {
                target = Target::Ios
            },
            "android" => {
                target = Target::Android
            },
            "web" => {
                target = Target::Web
            },
            _ => {
                println!("Unsupported target {}", target_s);
                return;
            }
        }

        println!("Running project in {} for target platform {}", dir, target.to_string());

        let path = Path::new(&dir);
        let entries = path.read_dir();
        match entries {
            Ok(entries) => {
                if entries.count() == 0 {
                    println!("Error: project directory is empty. Cannot update.");
                    return;
                }
            },
            Err(e) => {
                println!("Error: cannot read project folder, maybe it doesn't exist. {}", e.to_string());
                return;
            }
        }

        if !self.run_project(&dir, &target) {
            println!("Error: cannot run project");
            return;
        }

        println!("Project run.");
    }

    pub fn run_project(&self, dir: &String, target: &Target) -> bool {
        let path = Path::new(&dir);
        let project_path = path.join("binocle.json");
        println!("Looking for project file {}", project_path.to_string_lossy());
        let project_file = File::open(project_path);
        match project_file {
            Ok(mut project_file) => {
                let mut project_content = String::new();
                let read_size = project_file.read_to_string(&mut project_content);
                match read_size {
                    Ok(_read_size) => {
                        println!("{}", project_content);
                        let project_json: Value = serde_json::from_str(project_content.as_str()).unwrap();
                        let engine = &project_json["project"]["engine"];
                        if engine != "binocle-c" && engine != "binocle" {
                            println!("Error: wrong engine specified: {}", engine);
                            return false;
                        }
                        let ios_development_team_id = &project_json["project"]["ios_development_team_id"];
                        let team_id = format!("IOS_DEVELOPMENT_TEAM_ID={}", ios_development_team_id);
                        let team_id_s = team_id.as_str();
                        let project_name = &project_json["project"]["name"].as_str().unwrap();
                        match target {
                            Target::Mac => {
                                let command = "open".to_string();
                                let filename = format!("{}.app", project_name);
                                let app_path = Path::new(&filename);
                                let app_path_s = app_path.to_str().unwrap();
                                let app_name = project_name.to_string();
                                let app_name_s = app_name.as_str();
                                let parameters = [app_name_s];
                                let cwd = path.join("build").join(target.to_string()).join("src").join(&filename).join("Contents").join("MacOS");
                                let cwd_s = cwd.to_str().unwrap().to_string();
                                println!("Running application {} in dir {}", project_name, cwd_s);
                                self.execute_command(&command, &parameters, &cwd_s);
                            },
                            Target::Windows => {
                                println!("Do not know how to run for this target. Please submit a PR if you know what to do");
                                return false;
                            },
                            Target::Linux => {
                                println!("Do not know how to run for this target. Please submit a PR if you know what to do");
                                return false;
                            },
                            Target::Ios => {
                                let ios_bundle_id = &project_json["project"]["ios_bundle_id"].as_str().unwrap();
                                let device_id = self.find_device_id(&dir);
                                let filename = format!("{}.app", project_name);
                                let application_path = std::fs::canonicalize(Path::new(dir).join("build").join(target.to_string()).join("src").join(filename)).unwrap();
                                let application_path_s = application_path.to_str().unwrap().to_string();
                                match device_id {
                                    Some(device_id) => {
                                        println!("Opening simulator with device id {}", device_id);
                                        self.execute_command(&"open".to_string(), &["-a", "Simulator", "--args", "-CurrentDeviceUDID", &device_id].to_vec(), &dir);
                                        std::thread::sleep(Duration::from_secs(3));
                                        println!("Uninstalling application {}", ios_bundle_id);
                                        self.execute_command(&"xcrun".to_string(), &["simctl", "uninstall", &device_id, &ios_bundle_id].to_vec(), &dir);
                                        std::thread::sleep(Duration::from_secs(3));
                                        println!("Installing application {}", application_path_s);
                                        self.execute_command(&"xcrun".to_string(), &["simctl", "install", &device_id, &application_path_s].to_vec(), &dir);
                                        std::thread::sleep(Duration::from_secs(3));
                                        println!("Launching application {}", ios_bundle_id);
                                        self.execute_command(&"xcrun".to_string(), &["simctl", "launch", &device_id, &ios_bundle_id].to_vec(), &dir);
                                    }
                                    None => {
                                        return false;
                                    }
                                }
                            },
                            Target::Android => {
                                println!("Do not know how to run for this target. Please submit a PR if you know what to do");
                                return false;
                            },
                            Target::Web => {
                                println!("Do not know how to run for this target. Please submit a PR if you know what to do");
                                return false;
                            }
                            Target::Unknown => {
                                println!("Do not know how to run for this target. Please submit a PR if you know what to do");
                                return false;
                            }
                        }
                    },
                    Err(e) => {
                        println!("Error: cannot read project file. {}", e.to_string());
                        return false;
                    },
                }
            },
            Err(e) => {
                println!("Error: cannot open project file. {}", e.to_string());
                return false;
            }
        }
        true
    }

    pub fn execute_command(&self, command: &String, parameters: &[&str], cwd: &String) {
        let output = Command::new(command)
        .args(parameters)
        .current_dir(cwd)
        .output()
        .expect("Failed to run command");
        println!("{}", String::from_utf8(output.stdout).unwrap());
        println!("{}", String::from_utf8(output.stderr).unwrap());
    }

    pub fn execute_command_with_output(&self, command: &String, parameters: &[&str], cwd: &String) -> String {
        let output = Command::new(command)
        .args(parameters)
        .current_dir(cwd)
        .output()
        .expect("Failed to run command");
        return String::from_utf8(output.stdout).unwrap();
    }

    pub fn find_device_id(&self, cwd: &String) -> Option<String> {
        let command = "xcrun".to_string();
        let parameters = ["simctl", "list", "-j", "devices"].to_vec();
        let output = self.execute_command_with_output(&command, &parameters, cwd);
        let simulators: Simulators = serde_json::from_str(output.as_str()).unwrap();
        let devices = simulators.devices;
        for device in devices {
            if device.0.contains("iOS") {
                return Some(device.1[0].udid.clone());
            }
        }
        return None;
    }


}