# If there is no version tag in git this one will be used
VERSION = 0.2.0

# Need to discard STDERR so get path to NULL device
win32 {
    NULL_DEVICE = NUL # Windows doesn't have /dev/null but has NUL
} else {
    NULL_DEVICE = /dev/null
}

# Need to call git with manually specified paths to repository
# BASE_GIT_COMMAND = c:\Program Files (x86)\Git\bin\git --git-dir $$PWD/../.git --work-tree $$PWD/..
win32 {
BASE_GIT_COMMAND = "c:\Program Files\Git\bin\git"
} else {
BASE_GIT_COMMAND = "git"
}

message(The current dir is $$PWD)

message(command: \"$$BASE_GIT_COMMAND\" describe --always --tags)

# Trying to get version from git tag / revision
GIT_VERSION = $$system(\"$$BASE_GIT_COMMAND\" describe --always --tags 2> $$NULL_DEVICE)

message(git returns: $$GIT_VERSION)

# Check if we only have hash without version number
!contains(GIT_VERSION,\d+\.\d+\.\d+) {
    # If there is nothing we simply use version defined manually
    isEmpty(GIT_VERSION) {
        GIT_VERSION = $$VERSION
    } else { # otherwise construct proper git describe string
        GIT_COMMIT_COUNT = $$system($$BASE_GIT_COMMAND rev-list HEAD --count 2> $$NULL_DEVICE)
        isEmpty(GIT_COMMIT_COUNT) {
            GIT_COMMIT_COUNT = 0
        }
        GIT_VERSION = $$GIT_VERSION
    }
}

# Turns describe output like 0.1.5-42-g652c397 into "0.1.5.42.652c397"
GIT_VERSION ~= s/-/"."
GIT_VERSION ~= s/g/""

# Now we are ready to pass parsed version to Qt
VERSION = $$GIT_VERSION
win32 { # On windows version can only be numerical so remove commit hash
    VERSION ~= s/\.\d+\.[a-f0-9]{6,}//
# Take out the v from the version
    VERSION ~= s/v//
}

# Adding C preprocessor #DEFINE so we can use it in C++ code
# also here we want full version on every system so using GIT_VERSION
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

message(version: $$GIT_VERSION)
message(qt-version: $$VERSION)


