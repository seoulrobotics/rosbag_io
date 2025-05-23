/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2008, Willow Garage, Inc.
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

#ifndef ROSBAG_BAG_H
#define ROSBAG_BAG_H

#include "rosbag_io/rosbag/macros.h"

#include "rosbag_io/rosbag/buffer.h"
#include "rosbag_io/rosbag/chunked_file.h"
#include "rosbag_io/rosbag/constants.h"
#include "rosbag_io/rosbag/encryptor.h"
#include "rosbag_io/rosbag/exceptions.h"
#include "rosbag_io/rosbag/structures.h"

#include "rosbag_io/ros/header.h"
#include "rosbag_io/ros/time.h"
#include "rosbag_io/ros/message_traits.h"
#include "rosbag_io/ros/message_event.h"
#include "rosbag_io/ros/serialization.h"

#include <ios>
#include <map>
#include <queue>
#include <set>
#include <stdexcept>

#include <boost/config.hpp>
#include <boost/format.hpp>
#include <boost/iterator/iterator_facade.hpp>

#if defined logDebug
# undef logDebug
#endif
#if defined logInform
# undef logInform
#endif
#if defined logWarn
# undef logWarn
#endif
#if defined logError
# undef logError
#endif

namespace rosbag_io {
namespace rosbag {

namespace bagmode
{
    //! The possible modes to open a bag in
    enum BagMode
    {
        Write   = 1,
        Read    = 2,
        Append  = 4
    };
}
typedef bagmode::BagMode BagMode;

class MessageInstance;
class View;
class Query;

class ROSBAG_STORAGE_DECL Bag
{
    friend class MessageInstance;
    friend class View;

public:
    Bag();

    //! Open a bag file
    /*!
     * \param filename The bag file to open
     * \param mode     The mode to use (either read, write or append)
     *
     * Can throw BagException
     */
    explicit Bag(std::string const& filename, uint32_t mode = bagmode::Read);

    ~Bag();

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    Bag(Bag&& other);

    Bag& operator=(Bag&& other);
#endif // BOOST_NO_CXX11_RVALUE_REFERENCES

    //! Open a bag file.
    /*!
     * \param filename The bag file to open
     * \param mode     The mode to use (either read, write or append)
     *
     * Can throw BagException
     */
    void open(std::string const& filename, uint32_t mode = bagmode::Read);

    //! Close the bag file
    void close();

    std::string     getFileName()     const;                      //!< Get the filename of the bag
    BagMode         getMode()         const;                      //!< Get the mode the bag is in
    uint32_t        getMajorVersion() const;                      //!< Get the major-version of the open bag file
    uint32_t        getMinorVersion() const;                      //!< Get the minor-version of the open bag file
    uint64_t        getSize()         const;                      //!< Get the current size of the bag file (a lower bound)

    void            setCompression(CompressionType compression);  //!< Set the compression method to use for writing chunks
    CompressionType getCompression() const;                       //!< Get the compression method to use for writing chunks
    void            setChunkThreshold(uint32_t chunk_threshold);  //!< Set the threshold for creating new chunks
    uint32_t        getChunkThreshold() const;                    //!< Get the threshold for creating new chunks

    //! Set encryptor of the bag file
    /*!
     * \param plugin_name The name of the encryptor plugin
     * \param plugin_param The string parameter to be passed to the plugin initialization method
     *
     * Call this method to specify an encryptor for writing bag contents. This method need not be called when
     * reading or appending a bag file: The encryptor is read from the bag file header.
     */
    void setEncryptorPlugin(const std::string& plugin_name, const std::string& plugin_param = std::string());

    //! Write a message into the bag file
    /*!
     * \param topic The topic name
     * \param event The message event to be added
     *
     * Can throw BagIOException
     */
    template<class T>
    void write(std::string const& topic, ros::MessageEvent<T> const& event);

    //! Write a message into the bag file
    /*!
     * \param topic The topic name
     * \param time  Timestamp of the message
     * \param msg   The message to be added
     * \param connection_header  A connection header.
     *
     * Can throw BagIOException
     */
    template<class T>
    void write(std::string const& topic, ros::Time const& time, T const& msg,
               boost::shared_ptr<ros::M_string> connection_header = boost::shared_ptr<ros::M_string>());

    //! Write a message into the bag file
    /*!
     * \param topic The topic name
     * \param time  Timestamp of the message
     * \param msg   The message to be added
     * \param connection_header  A connection header.
     *
     * Can throw BagIOException
     */
    template<class T>
    void write(std::string const& topic, ros::Time const& time, boost::shared_ptr<T const> const& msg,
               boost::shared_ptr<ros::M_string> connection_header = boost::shared_ptr<ros::M_string>());

    //! Write a message into the bag file
    /*!
     * \param topic The topic name
     * \param time  Timestamp of the message
     * \param msg   The message to be added
     * \param connection_header  A connection header.
     *
     * Can throw BagIOException
     */
    template<class T>
    void write(std::string const& topic, ros::Time const& time, boost::shared_ptr<T> const& msg,
               boost::shared_ptr<ros::M_string> connection_header = boost::shared_ptr<ros::M_string>());

    void swap(Bag&);

    bool isOpen() const;

private:
    // disable copying
    Bag(const Bag&);
    Bag& operator=(const Bag&);

    void init();

    // This helper function actually does the write with an arbitrary serializable message
    template<class T>
    void doWrite(std::string const& topic, ros::Time const& time, T const& msg, boost::shared_ptr<ros::M_string> const& connection_header);

    void openRead  (std::string const& filename);
    void openWrite (std::string const& filename);
    void openAppend(std::string const& filename);

    void closeWrite();

    template<class T>
    boost::shared_ptr<T> instantiateBuffer(IndexEntry const& index_entry) const;  //!< deserializes the message held in record_buffer_

    void startWriting();
    void stopWriting();

    void startReadingVersion102();
    void startReadingVersion200();

    // Writing
    
    void writeVersion();
    void writeFileHeaderRecord();
    void writeConnectionRecord(ConnectionInfo const* connection_info, const bool encrypt);
    void appendConnectionRecordToBuffer(Buffer& buf, ConnectionInfo const* connection_info);
    template<class T>
    void writeMessageDataRecord(uint32_t conn_id, ros::Time const& time, T const& msg);
    void writeIndexRecords();
    void writeConnectionRecords();
    void writeChunkInfoRecords();
    void startWritingChunk(ros::Time time);
    void writeChunkHeader(CompressionType compression, uint32_t compressed_size, uint32_t uncompressed_size);
    void stopWritingChunk();

    // Reading

    void readVersion();
    void readFileHeaderRecord();
    void readConnectionRecord();
    void readChunkHeader(ChunkHeader& chunk_header) const;
    void readChunkInfoRecord();
    void readConnectionIndexRecord200();

    void readTopicIndexRecord102();
    void readMessageDefinitionRecord102();
    void readMessageDataRecord102(uint64_t offset, ros::Header& header) const;

    ros::Header readMessageDataHeader(IndexEntry const& index_entry);
    uint32_t    readMessageDataSize(IndexEntry const& index_entry) const;

    template<typename Stream>
    void readMessageDataIntoStream(IndexEntry const& index_entry, Stream& stream) const;

    void     decompressChunk(uint64_t chunk_pos) const;
    void     decompressRawChunk(ChunkHeader const& chunk_header) const;
    void     decompressBz2Chunk(ChunkHeader const& chunk_header) const;
    void     decompressLz4Chunk(ChunkHeader const& chunk_header) const;
    uint32_t getChunkOffset() const;

    // Record header I/O

    void writeHeader(ros::M_string const& fields);
    void writeDataLength(uint32_t data_len);
    void appendHeaderToBuffer(Buffer& buf, ros::M_string const& fields);
    void appendDataLengthToBuffer(Buffer& buf, uint32_t data_len);

    void readHeaderFromBuffer(Buffer& buffer, uint32_t offset, ros::Header& header, uint32_t& data_size, uint32_t& bytes_read) const;
    void readMessageDataHeaderFromBuffer(Buffer& buffer, uint32_t offset, ros::Header& header, uint32_t& data_size, uint32_t& bytes_read) const;
    bool readHeader(ros::Header& header) const;
    bool readDataLength(uint32_t& data_size) const;
    bool isOp(ros::M_string& fields, uint8_t reqOp) const;

    // Header fields

    template<typename T>
    std::string toHeaderString(T const* field) const;

    std::string toHeaderString(ros::Time const* field) const;

    template<typename T>
    bool readField(ros::M_string const& fields, std::string const& field_name, bool required, T* data) const;

    bool readField(ros::M_string const& fields, std::string const& field_name, unsigned int min_len, unsigned int max_len, bool required, std::string& data) const;
    bool readField(ros::M_string const& fields, std::string const& field_name, bool required, std::string& data) const;

    bool readField(ros::M_string const& fields, std::string const& field_name, bool required, ros::Time& data) const;

    ros::M_string::const_iterator checkField(ros::M_string const& fields, std::string const& field,
                                             unsigned int min_len, unsigned int max_len, bool required) const;

    // Low-level I/O

    void write(char const* s, std::streamsize n);
    void write(std::string const& s);
    void read(char* b, std::streamsize n) const;
    void seek(uint64_t pos, int origin = std::ios_base::beg) const;

private:
    BagMode             mode_;
    mutable ChunkedFile file_;
    int                 version_;
    CompressionType     compression_;
    uint32_t            chunk_threshold_;
    uint32_t            bag_revision_;

    uint64_t file_size_;
    uint64_t file_header_pos_;
    uint64_t index_data_pos_;
    uint32_t connection_count_;
    uint32_t chunk_count_;
    
    // Current chunk
    bool      chunk_open_;
    ChunkInfo curr_chunk_info_;
    uint64_t  curr_chunk_data_pos_;

    std::map<std::string, uint32_t>                topic_connection_ids_;
    std::map<ros::M_string, uint32_t>              header_connection_ids_;
    std::map<uint32_t, ConnectionInfo*>            connections_;

    std::vector<ChunkInfo>                         chunks_;

    std::map<uint32_t, std::multiset<IndexEntry> > connection_indexes_;
    std::map<uint32_t, std::multiset<IndexEntry> > curr_chunk_connection_indexes_;

    mutable Buffer   header_buffer_;           //!< reusable buffer in which to assemble the record header before writing to file
    mutable Buffer   record_buffer_;           //!< reusable buffer in which to assemble the record data before writing to file

    mutable Buffer   chunk_buffer_;            //!< reusable buffer to read chunk into
    mutable Buffer   decompress_buffer_;       //!< reusable buffer to decompress chunks into

    mutable Buffer   outgoing_chunk_buffer_;   //!< reusable buffer to read chunk into

    mutable Buffer*  current_buffer_;

    mutable uint64_t decompressed_chunk_;      //!< position of decompressed chunk

    // Active encryptor
    boost::shared_ptr<rosbag::EncryptorBase> encryptor_;
};

} // namespace rosbag
} // namespace rosbag_io

#include "rosbag_io/rosbag/message_instance.h"

namespace rosbag_io {
namespace rosbag {

// Templated method definitions

template<class T>
void Bag::write(std::string const& topic, ros::MessageEvent<T> const& event) {
    doWrite(topic, event.getReceiptTime(), *event.getMessage(), event.getConnectionHeaderPtr());
}

template<class T>
void Bag::write(std::string const& topic, ros::Time const& time, T const& msg, boost::shared_ptr<ros::M_string> connection_header) {
    doWrite(topic, time, msg, connection_header);
}

template<class T>
void Bag::write(std::string const& topic, ros::Time const& time, boost::shared_ptr<T const> const& msg, boost::shared_ptr<ros::M_string> connection_header) {
    doWrite(topic, time, *msg, connection_header);
}

template<class T>
void Bag::write(std::string const& topic, ros::Time const& time, boost::shared_ptr<T> const& msg, boost::shared_ptr<ros::M_string> connection_header) {
    doWrite(topic, time, *msg, connection_header);
}

template<typename T>
std::string Bag::toHeaderString(T const* field) const {
    return std::string((char*) field, sizeof(T));
}

template<typename T>
bool Bag::readField(ros::M_string const& fields, std::string const& field_name, bool required, T* data) const {
    ros::M_string::const_iterator i = checkField(fields, field_name, sizeof(T), sizeof(T), required);
    if (i == fields.end())
    	return false;
    memcpy(data, i->second.data(), sizeof(T));
    return true;
}

template<typename Stream>
void Bag::readMessageDataIntoStream(IndexEntry const& index_entry, Stream& stream) const {
    ros::Header header;
    uint32_t data_size;
    uint32_t bytes_read;
    switch (version_)
    {
    case 200:
    {
        decompressChunk(index_entry.chunk_pos);
        readMessageDataHeaderFromBuffer(*current_buffer_, index_entry.offset, header, data_size, bytes_read);
        if (data_size > 0)
            memcpy(stream.advance(data_size), current_buffer_->getData() + index_entry.offset + bytes_read, data_size);
        break;
    }
    case 102:
    {
        readMessageDataRecord102(index_entry.chunk_pos, header);
        data_size = record_buffer_.getSize();
        if (data_size > 0)
            memcpy(stream.advance(data_size), record_buffer_.getData(), data_size);
        break;
    }
    default:
        throw BagFormatException((boost::format("Unhandled version: %1%") % version_).str());
    }
}

template<class T>
boost::shared_ptr<T> Bag::instantiateBuffer(IndexEntry const& index_entry) const {
    switch (version_)
    {
    case 200:
	{
        decompressChunk(index_entry.chunk_pos);

        // Read the message header
        ros::Header header;
        uint32_t data_size;
        uint32_t bytes_read;
        readMessageDataHeaderFromBuffer(*current_buffer_, index_entry.offset, header, data_size, bytes_read);

        // Read the connection id from the header
        uint32_t connection_id;
        readField(*header.getValues(), CONNECTION_FIELD_NAME, true, &connection_id);

        std::map<uint32_t, ConnectionInfo*>::const_iterator connection_iter = connections_.find(connection_id);
        if (connection_iter == connections_.end())
            throw BagFormatException((boost::format("Unknown connection ID: %1%") % connection_id).str());
        ConnectionInfo* connection_info = connection_iter->second;

        boost::shared_ptr<T> p = boost::make_shared<T>();

        ros::serialization::PreDeserializeParams<T> predes_params;
        predes_params.message = p;
        predes_params.connection_header = connection_info->header;
        ros::serialization::PreDeserialize<T>::notify(predes_params);

        // Deserialize the message
        ros::serialization::IStream s(current_buffer_->getData() + index_entry.offset + bytes_read, data_size);
        ros::serialization::deserialize(s, *p);

        return p;
	}
    case 102:
	{
        // Read the message record
        ros::Header header;
        readMessageDataRecord102(index_entry.chunk_pos, header);

        ros::M_string& fields = *header.getValues();

        // Read the connection id from the header
        std::string topic, latching("0"), callerid;
        readField(fields, TOPIC_FIELD_NAME,    true,  topic);
        readField(fields, LATCHING_FIELD_NAME, false, latching);
        readField(fields, CALLERID_FIELD_NAME, false, callerid);

        std::map<std::string, uint32_t>::const_iterator topic_conn_id_iter = topic_connection_ids_.find(topic);
        if (topic_conn_id_iter == topic_connection_ids_.end())
            throw BagFormatException((boost::format("Unknown topic: %1%") % topic).str());
        uint32_t connection_id = topic_conn_id_iter->second;

        std::map<uint32_t, ConnectionInfo*>::const_iterator connection_iter = connections_.find(connection_id);
        if (connection_iter == connections_.end())
            throw BagFormatException((boost::format("Unknown connection ID: %1%") % connection_id).str());
        ConnectionInfo* connection_info = connection_iter->second;

        boost::shared_ptr<T> p = boost::make_shared<T>();

        // Create a new connection header, updated with the latching and callerid values
        boost::shared_ptr<ros::M_string> message_header(boost::make_shared<ros::M_string>());
        for (ros::M_string::const_iterator i = connection_info->header->begin(); i != connection_info->header->end(); i++)
            (*message_header)[i->first] = i->second;
        (*message_header)["latching"] = latching;
        (*message_header)["callerid"] = callerid;

        ros::serialization::PreDeserializeParams<T> predes_params;
        predes_params.message = p;
        predes_params.connection_header = message_header;
        ros::serialization::PreDeserialize<T>::notify(predes_params);

        // Deserialize the message
        ros::serialization::IStream s(record_buffer_.getData(), record_buffer_.getSize());
        ros::serialization::deserialize(s, *p);

        return p;
	}
    default:
        throw BagFormatException((boost::format("Unhandled version: %1%") % version_).str());
    }
}

template<class T>
void Bag::doWrite(std::string const& topic, ros::Time const& time, T const& msg, boost::shared_ptr<ros::M_string> const& connection_header) {

    if (time < ros::TIME_MIN)
    {
        throw BagException("Tried to insert a message with time less than ros::TIME_MIN");
    }

    // Whenever we write we increment our revision
    bag_revision_++;

    // Get ID for connection header
    ConnectionInfo* connection_info = NULL;
    uint32_t conn_id = 0;
    if (!connection_header) {
        // No connection header: we'll manufacture one, and store by topic

        std::map<std::string, uint32_t>::iterator topic_connection_ids_iter = topic_connection_ids_.find(topic);
        if (topic_connection_ids_iter == topic_connection_ids_.end()) {
            conn_id = connections_.size();
            topic_connection_ids_[topic] = conn_id;
        }
        else {
            conn_id = topic_connection_ids_iter->second;
            connection_info = connections_[conn_id];
        }
    }
    else {
        // Store the connection info by the address of the connection header

        // Add the topic name to the connection header, so that when we later search by 
        // connection header, we can disambiguate connections that differ only by topic name (i.e.,
        // same callerid, same message type), #3755.  This modified connection header is only used
        // for our bookkeeping, and will not appear in the resulting .bag.
        ros::M_string connection_header_copy(*connection_header);
        connection_header_copy["topic"] = topic;

        std::map<ros::M_string, uint32_t>::iterator header_connection_ids_iter = header_connection_ids_.find(connection_header_copy);
        if (header_connection_ids_iter == header_connection_ids_.end()) {
            conn_id = connections_.size();
            header_connection_ids_[connection_header_copy] = conn_id;
        }
        else {
            conn_id = header_connection_ids_iter->second;
            connection_info = connections_[conn_id];
        }
    }

    {
        // Seek to the end of the file (needed in case previous operation was a read)
        seek(0, std::ios::end);
        file_size_ = file_.getOffset();

        // Write the chunk header if we're starting a new chunk
        if (!chunk_open_)
            startWritingChunk(time);

        // Write connection info record, if necessary
        if (connection_info == NULL) {
            connection_info = new ConnectionInfo();
            connection_info->id       = conn_id;
            connection_info->topic    = topic;
            connection_info->datatype = std::string(ros::message_traits::datatype(msg));
            connection_info->md5sum   = std::string(ros::message_traits::md5sum(msg));
            connection_info->msg_def  = std::string(ros::message_traits::definition(msg));
            if (connection_header != NULL) {
                connection_info->header = connection_header;
            }
            else {
                connection_info->header = boost::make_shared<ros::M_string>();
                (*connection_info->header)["type"]               = connection_info->datatype;
                (*connection_info->header)["md5sum"]             = connection_info->md5sum;
                (*connection_info->header)["message_definition"] = connection_info->msg_def;
            }
            connections_[conn_id] = connection_info;
            // No need to encrypt connection records in chunks
            writeConnectionRecord(connection_info, false);
            appendConnectionRecordToBuffer(outgoing_chunk_buffer_, connection_info);
        }

        // Add to topic indexes
        IndexEntry index_entry;
        index_entry.time      = time;
        index_entry.chunk_pos = curr_chunk_info_.pos;
        index_entry.offset    = getChunkOffset();

        std::multiset<IndexEntry>& chunk_connection_index = curr_chunk_connection_indexes_[connection_info->id];
        chunk_connection_index.insert(chunk_connection_index.end(), index_entry);

        if (mode_ != BagMode::Write) {
          std::multiset<IndexEntry>& connection_index = connection_indexes_[connection_info->id];
          connection_index.insert(connection_index.end(), index_entry);
        }

        // Increment the connection count
        curr_chunk_info_.connection_counts[connection_info->id]++;

        // Write the message data
        writeMessageDataRecord(conn_id, time, msg);

        // Check if we want to stop this chunk
        uint32_t chunk_size = getChunkOffset();
        if (chunk_size > chunk_threshold_) {
            // Empty the outgoing chunk
            stopWritingChunk();
            outgoing_chunk_buffer_.setSize(0);

            // We no longer have a valid curr_chunk_info
            curr_chunk_info_.pos = -1;
        }
    }
}

template<class T>
void Bag::writeMessageDataRecord(uint32_t conn_id, ros::Time const& time, T const& msg) {
    ros::M_string header;
    header[OP_FIELD_NAME]         = toHeaderString(&OP_MSG_DATA);
    header[CONNECTION_FIELD_NAME] = toHeaderString(&conn_id);
    header[TIME_FIELD_NAME]       = toHeaderString(&time);

    // Assemble message in memory first, because we need to write its length
    uint32_t msg_ser_len = ros::serialization::serializationLength(msg);

    record_buffer_.setSize(msg_ser_len);
    
    ros::serialization::OStream s(record_buffer_.getData(), msg_ser_len);

    // todo: serialize into the outgoing_chunk_buffer & remove record_buffer_
    ros::serialization::serialize(s, msg);

    // We do an extra seek here since writing our data record may
    // have indirectly moved our file-pointer if it was a
    // MessageInstance for our own bag
    seek(0, std::ios::end);
    file_size_ = file_.getOffset();

    writeHeader(header);
    writeDataLength(msg_ser_len);
    write((char*) record_buffer_.getData(), msg_ser_len);
    
    // todo: use better abstraction than appendHeaderToBuffer
    appendHeaderToBuffer(outgoing_chunk_buffer_, header);
    appendDataLengthToBuffer(outgoing_chunk_buffer_, msg_ser_len);

    uint32_t offset = outgoing_chunk_buffer_.getSize();
    outgoing_chunk_buffer_.setSize(outgoing_chunk_buffer_.getSize() + msg_ser_len);
    memcpy(outgoing_chunk_buffer_.getData() + offset, record_buffer_.getData(), msg_ser_len);
    
    // Update the current chunk time range
    if (time > curr_chunk_info_.end_time)
    	curr_chunk_info_.end_time = time;
    else if (time < curr_chunk_info_.start_time)
        curr_chunk_info_.start_time = time;
}

inline void swap(Bag& a, Bag& b) {
    a.swap(b);
}

} // namespace rosbag
} // namespace rosbag_io

#endif
