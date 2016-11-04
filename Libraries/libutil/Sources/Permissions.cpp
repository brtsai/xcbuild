/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/Permissions.h>

using libutil::Permissions;

void Permissions::
combine(Operation operation, Permissions other)
{
    switch (operation) {
        case Operation::Set:
            _flags = other._flags;
            _user = other._user;
            _group = other._group;
            _other = other._other;
            return;
        case Operation::Add:
            _flags |= other._flags;
            _user |= other._user;
            _group |= other._group;
            _other |= other._other;
            return;
        case Operation::Remove:
            _flags &= ~other._flags;
            _user &= ~other._user;
            _group &= ~other._group;
            _other &= ~other._other;
            return;
    }

    abort();
}