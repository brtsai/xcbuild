/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

#include <unordered_set>
#include <sstream>

using libutil::Filesystem;
using libutil::FSUtil;

bool Filesystem::
copyFile(std::string const &from, std::string const &to)
{
    std::vector<uint8_t> contents;

    if (!this->read(&contents, from)) {
        return false;
    }

    if (!this->write(contents, to)) {
        return false;
    }

    return true;
}

bool Filesystem::
copySymbolicLink(std::string const &from, std::string const &to)
{
    ext::optional<std::string> target = this->readSymbolicLink(from);
    if (!target) {
        return false;
    }

    /*
     * Remove any existing symbolic link to overwrite, in the same way that copying
     * a file overwrites an existing file at the same path, but not a directory.
     */
    if (this->isSymbolicLink(to)) {
        if (!this->removeSymbolicLink(to)) {
            return false;
        }
    }

    if (!this->writeSymbolicLink(to, *target)) {
        return false;
    }

    return true;
}

bool Filesystem::
copyDirectory(std::string const &from, std::string const &to, bool recursive)
{
    if (!this->isDirectory(from)) {
        return false;
    }

    /*
     * Remove any existing directory to overwrite, in the same way that copying
     * a file overwrites an existing file at the same path, but not a directory.
     */
    if (this->isDirectory(to)) {
        if (!this->removeDirectory(to, recursive)) {
            return false;
        }
    }

    if (!this->createDirectory(to, false)) {
        return false;
    }

    if (recursive) {
        bool success = true;

        success &= this->readDirectory(from, recursive, [this, &from, &to, &success](std::string const &path) {
            std::string fromPath = from + "/" + path;
            std::string toPath = to + "/" + path;

            if (this->isSymbolicLink(fromPath)) {
                if (!this->copySymbolicLink(fromPath, toPath)) {
                    success = false;
                    return false;
                }
            } else if (this->isDirectory(fromPath)) {
                if (!this->copyDirectory(fromPath, toPath, false)) {
                    success = false;
                    return false;
                }
            } else if (this->isFile(fromPath)) {
                if (!this->copyFile(fromPath, toPath)) {
                    success = false;
                    return false;
                }
            } else {
                /* Unknown entry type; can't copy. */
            }

            return true;
        });

        if (!success) {
            return false;
        }
    }

    return true;
}

ext::optional<std::string> Filesystem::
findFile(std::string const &name, std::vector<std::string> const &paths) const
{
    if (name.empty()) {
        return ext::nullopt;
    }

    for (auto const &path : paths) {
        std::string filePath = path + "/" + name;
        if (this->exists(filePath)) {
            return FSUtil::NormalizePath(filePath);
        }
    }

    return ext::nullopt;
}

ext::optional<std::string> Filesystem::
findExecutable(std::string const &name, std::vector<std::string> const &paths) const
{
    ext::optional<std::string> exePath = findFile(name, paths);

    if (!exePath) {
        return ext::nullopt;
    }

    if (this->isExecutable(*exePath)) {
        return FSUtil::NormalizePath(*exePath);
    }

    return ext::nullopt;
}

#include <libutil/DefaultFilesystem.h>

Filesystem *Filesystem::
GetDefaultUNSAFE()
{
    static DefaultFilesystem *filesystem = nullptr;
    if (filesystem == nullptr) {
        filesystem = new DefaultFilesystem();
    }

    return filesystem;
}
