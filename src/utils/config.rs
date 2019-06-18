use std::fmt::Display;
use std::fmt::Formatter;
use std::fmt::Result;

pub enum Target {
    Mac,
    Windows,
    Linux,
    Android,
    Ios,
    Web,
}

impl Display for Target {
    fn fmt(&self, f: &mut Formatter) -> Result {
        match self {
            Target::Mac => {
                write!(f, "mac")
            }
            Target::Windows => {
                write!(f, "windows")
            }
            Target::Linux => {
                write!(f, "linux")
            }
            Target::Android => {
                write!(f, "android")
            }
            Target::Ios => {
                write!(f, "ios")
            }
            Target::Web => {
                write!(f, "web")
            }
        }
    }
}

