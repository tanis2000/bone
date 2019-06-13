pub enum Target {
    Mac,
    Windows,
    Linux,
    Android,
    Ios,
    Web,
    Unknown,
}

impl std::string::ToString for Target {
    fn to_string(&self) -> std::string::String {
        match self {
            Mac => {
                "mac".to_string()
            }
            Windows => {
                "windows".to_string()
            }
            Linux => {
                "linux".to_string()
            }
            Android => {
                "android".to_string()
            }
            Ios => {
                "ios".to_string()
            }
            Web => {
                "web".to_string()
            }
            _ => {
                "unknown".to_string()
            }
        }
    }
}

pub struct BuildConfig {
    supported_targets: Vec<Target>,

}

pub struct Config {
    build: BuildConfig,
}

impl Config {
    pub fn default() -> Self {
        let targets = vec![Target::Mac, Target::Windows, Target::Linux, Target::Android, Target::Ios, Target::Web];
        Config {
            build: BuildConfig {
                supported_targets: targets
            }
        }
    }
} 