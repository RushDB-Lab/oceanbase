/**
 * Copyright (c) 2023 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#pragma once
#include "observer/table_load/backup/v_1_4/ob_table_load_backup_row_reader.h"

namespace oceanbase
{
namespace observer
{
namespace table_load_backup_v_1_4
{

class ObTableLoadBackupMicroBlockRecordHeader
{
public:
  static const int8_t PRE_MICRO_BLOCK_RECORD_HEADER_VERSION = 0x2;
  static const int64_t PRE_MICRO_BLOCK_RECORD_HEADER_MAGIC = 1005;
  ObTableLoadBackupMicroBlockRecordHeader()
  {
    memset(this, 0, sizeof(ObTableLoadBackupMicroBlockRecordHeader));
    header_length_ = static_cast<int8_t>(sizeof(ObTableLoadBackupMicroBlockRecordHeader));
    version_ = ObTableLoadBackupMicroBlockRecordHeader::PRE_MICRO_BLOCK_RECORD_HEADER_VERSION;
  }
  ~ObTableLoadBackupMicroBlockRecordHeader() {}
  bool is_compressed_data() const { return data_length_ != data_zlength_; }
  int check_header_checksum() const;
  int check_payload_checksum(const char *buf, const int64_t len) const;
  static int check_record(const char *ptr, const int64_t size);
  TO_STRING_KV(K(magic_), K(header_length_), K(version_), K(header_checksum_),
               K(reserved16_), K(data_length_), K(data_zlength_), K(data_checksum_));
public:
  int16_t magic_;
  int8_t header_length_;
  int8_t version_;
  int16_t header_checksum_;
  int16_t reserved16_;
  int64_t data_length_;
  int64_t data_zlength_;
  int64_t data_checksum_;
};

class ObTableLoadBackupMicroBlockHeader_V_1_4
{
public:
  static const int64_t PRE_MICRO_BLOCK_HEADER_VERSION = 1;
  ObTableLoadBackupMicroBlockHeader_V_1_4()
  {
    memset(this, 0, sizeof(*this));
  }
  ~ObTableLoadBackupMicroBlockHeader_V_1_4() {}
  bool is_valid() const {
    return header_size_ > 0 && version_ >= PRE_MICRO_BLOCK_HEADER_VERSION &&
           magic_ == ObTableLoadBackupMicroBlockRecordHeader::PRE_MICRO_BLOCK_RECORD_HEADER_MAGIC &&
           attr_ >= 0 && column_count_ > 0 && row_index_offset_ > 0 && row_count_ > 0;
  }
  TO_STRING_KV(K(header_size_), K(version_), K(magic_), K(attr_), K(column_count_), K(row_index_offset_), K(row_count_));
public:
  int32_t header_size_;
  int32_t version_;
  int32_t magic_;
  int32_t attr_;//TODO:用途
  int32_t column_count_;
  int32_t row_index_offset_;
  int32_t row_count_;
};

class ObTableLoadBackupMicroBlockScanner
{
public:
  ObTableLoadBackupMicroBlockScanner()
    : allocator_("TLD_BMiBS_V14"),
      header_(nullptr),
      column_map_(nullptr),
      data_begin_(nullptr),
      index_begin_(nullptr),
      cur_idx_(0),
      is_inited_(false)
  {
    allocator_.set_tenant_id(MTL_ID());
  }
  ~ObTableLoadBackupMicroBlockScanner() {}
  int init(const char *buf,
           const ObTableLoadBackupColumnMap *column_map);
  void reset();
  void reuse();
  int get_next_row(ObNewRow *&row);
private:
  int init_row();
private:
  ObArenaAllocator allocator_;
  ObTableLoadBackupRowReader reader_;
  const ObTableLoadBackupMicroBlockHeader_V_1_4 *header_; //微块头首地址
  const ObTableLoadBackupColumnMap *column_map_;
  const char *data_begin_;
  const int32_t *index_begin_;
  common::ObNewRow row_;
  int32_t cur_idx_;
  bool is_inited_;
};

} // table_load_backup_v_1_4
} // namespace observer
} // namespace oceanbase
