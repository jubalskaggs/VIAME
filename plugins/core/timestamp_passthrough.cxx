/*ckwg +29
 * Copyright 2019 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "timestamp_passthrough.h"

#include <vital/algo/algorithm_factory.h>
#include <vital/plugin_loader/plugin_manager.h>

#include <sstream>

namespace viame
{

// ----------------------------------------------------------------------------
timestamp_passthrough::timestamp_passthrough()
{
  this->set_capability( kwiver::vital::algo::image_io::HAS_TIME, true );
}

// ----------------------------------------------------------------------------
kwiver::vital::config_block_sptr
  timestamp_passthrough::get_configuration() const
{
  auto config = kwiver::vital::algo::image_io::get_configuration();

  kwiver::vital::algo::image_io::get_nested_algo_configuration(
    "image_reader", config, this->image_reader);

  return config;
}

// ----------------------------------------------------------------------------
void timestamp_passthrough::set_configuration(
  kwiver::vital::config_block_sptr config )
{
  auto new_config = this->get_configuration();
  new_config->merge_config( config );

  kwiver::vital::algo::image_io::set_nested_algo_configuration(
    "image_reader", new_config, this->image_reader );
}

// ----------------------------------------------------------------------------
bool timestamp_passthrough::check_configuration(
  kwiver::vital::config_block_sptr config ) const
{
  return kwiver::vital::algo::image_io::check_nested_algo_configuration(
    "image_reader", config );
}

// ----------------------------------------------------------------------------
kwiver::vital::image_container_sptr timestamp_passthrough::load_(
  std::string const& filename ) const
{
  if( this->image_reader )
  {
    auto im = this->image_reader->load( filename );
    im->set_metadata( this->fixup_metadata( filename, im->get_metadata() ) );
    return im;
  }

  return nullptr;
}

// ----------------------------------------------------------------------------
void timestamp_passthrough::save_(
  std::string const& filename,
  kwiver::vital::image_container_sptr data ) const
{
  if( this->image_reader )
  {
    this->image_reader->save( filename, data );
  }
}

// ----------------------------------------------------------------------------
kwiver::vital::metadata_sptr timestamp_passthrough::load_metadata_(
  std::string const& filename) const
{
  if( this->image_reader )
  {
    return this->fixup_metadata( filename,
      this->image_reader->load_metadata( filename ) );
  }

  return this->fixup_metadata( filename, nullptr );
}

// ----------------------------------------------------------------------------
kwiver::vital::metadata_sptr timestamp_passthrough::fixup_metadata(
  std::string const& filename, kwiver::vital::metadata_sptr md ) const
{
  if( !md )
  {
    md = std::make_shared<kwiver::vital::metadata>();
  }

  unsigned utc_time_usec = 0;

  if( !utc_time_usec )
  {
    kwiver::vital::timestamp ts;
    ts.set_time_usec( utc_time_usec );
    md->set_timestamp( ts );
  }

  return md;
}

}
