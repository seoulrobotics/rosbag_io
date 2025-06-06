/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2017, Open Source Robotics Foundation
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of Willow Garage, Inc. nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#ifndef ROSBAG_NO_ENCRYPTION_H
#define ROSBAG_NO_ENCRYPTION_H

#include "rosbag_io/rosbag/encryptor.h"

namespace rosbag_io {
namespace rosbag {

class NoEncryptor : public EncryptorBase
{
public:
    NoEncryptor() { }
    ~NoEncryptor() { }

    void initialize(Bag const&, std::string const&) { }
    uint32_t encryptChunk(const uint32_t, const uint64_t, ChunkedFile&);
    void decryptChunk(ChunkHeader const&, Buffer&, ChunkedFile&) const;
    void addFieldsToFileHeader(ros::M_string&) const { }
    void readFieldsFromFileHeader(ros::M_string const&) { }
    void writeEncryptedHeader(boost::function<void(ros::M_string const&)>, ros::M_string const&, ChunkedFile&);
    bool readEncryptedHeader(boost::function<bool(ros::Header&)>, ros::Header&, Buffer&, ChunkedFile&);
};
}
}

#endif
