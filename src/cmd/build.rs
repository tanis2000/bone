use serde_json::{Result, Value};
use std::io;
use std::io::prelude::*;
use std::fs::{self, DirEntry, File};
use std::path::Path;
use std::process::Command;
use crate::utils::Processor;
use crate::utils::Target;

#[derive(Default, Builder, Debug)]
#[builder(setter(into))]
pub struct Build {
    app: Option<String>,
    dir: Option<String>,
    target: Option<String>,
}

impl Build {
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


        println!("Building project in {} for target platform {}", dir, target.to_string());

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

        if !self.build_project(&dir, &target) {
            println!("Error: cannot build project");
            return;
        }

        println!("Project built.");
    }

    pub fn build_project(&self, dir: &String, target: &Target) -> bool {
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
                        let mut command = "cmake".to_string();
                        let mut parameters: Vec<&str>;
                        let team_id = format!("IOS_DEVELOPMENT_TEAM_ID={}", ios_development_team_id);
                        let team_id_s = team_id.as_str();
                        match target {
                            Mac => {
                                parameters = ["-G", "Xcode", "-D", "DEBUG=1", "../.."].to_vec();
                            },
                            Windows => {
                                parameters = ["-G", "Unix Makefiles", "-D", "DEBUG=1", "../.."].to_vec();
                            },
                            Linux => {
                                parameters = ["-G", "Unix Makefiles", "-D", "DEBUG=1", "../.."].to_vec();
                            },
                            Ios => {
                                parameters = ["-G", "Xcode", "-D", "DEBUG=1", "-D", "IOS=1", "-D", team_id_s, "../.."].to_vec();
                            },
                            Android => {
                                parameters = ["-G", "Unix Makefiles", "-D", "DEBUG=1", "-D", "ANDROID_ABI=armeabi-v7a", "-D", "ANDROID_STL=gnustl_static", "-D", "ANDROID_NATIVE_API_LEVEL=android-17", "-D", "CMAKE_TOOLCHAIN_FILE=../Binocle/build/cmake/android.toolchain.cmake", "../.."].to_vec();
                            },
                            Web => {
                                command = "emcmake".to_string();
                                parameters = ["cmake", "../..", "-DCMAKE_BUILD_TYPE=Release"].to_vec();
                            }
                        }
                        let build_path_buf = path.join("build").join(target.to_string());
                        let build_path = build_path_buf.as_path();
                        std::fs::create_dir(build_path);
                        let build_path_s = build_path.to_str().unwrap().to_string();
                        self.execute_command(&command, &parameters, &build_path_s);

                        match target {
                            Mac => {
                                command = "xcodebuild".to_string();
                                parameters = [].to_vec();
                            },
                            Windows => {
                                println!("Do not know how to build for this target. Please submit a PR if you can help with Windows and Visual Studio scripting");
                                return false;
                            },
                            Linux => {
                                command = "make".to_string();
                                parameters = [].to_vec();
                            },
                            Ios => {
                                command = "xcodebuild".to_string();
                                parameters = ["-allowProvisioningUpdates", "-sdk", "iphonesimulator"].to_vec();
                            },
                            Android => {
                                command = "make".to_string();
                                parameters = [].to_vec();
                                self.execute_command(&command, &parameters, &build_path_s);
                                command = "gradlew".to_string();
                                parameters = ["assembleDebug"].to_vec();
                                let android_path = build_path.join("android-project");
                                let android_path_s = android_path.to_str().unwrap().to_string();
                                self.execute_command(&command, &parameters, &android_path_s);
                                return true;
                            },
                            Web => {
                                command = "make".to_string();
                                parameters = ["-j8"].to_vec();
                            },
                            _ => {
                                return false;
                            }
                        }
                        self.execute_command(&command, &parameters, &build_path_s);
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
    }


}