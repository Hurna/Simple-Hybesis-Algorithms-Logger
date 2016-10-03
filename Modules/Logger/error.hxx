/*===========================================================================================================
 *
 * SHA-L - Simple Hybesis Algorithm Logger
 *
 * Copyright (c) Michael Jeulin-Lagarrigue
 *
 *  Licensed under the MIT License, you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://github.com/michael-jeulinl/Simple-Hybesis-Algorithms-Logger/blob/master/LICENSE
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 *=========================================================================================================*/
#ifndef MODULE_LOGGER_ERROR_HXX
#define MODULE_LOGGER_ERROR_HXX

#include <Logger/typedef.hxx>

namespace SHA_Logger
{
  /// @class Error parameter
  ///
  class Error
  {
    public:
      // Assert correct JSON construction.
      ~Error() { assert(this->writer.IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument and write value information.
      ///
      /// @return stream reference filled up with Error object information,
      ///         error information in case of failure.
      static std::ostream& Build(std::ostream& os, String_Type& file, int line, String_Type& message)
      {
        auto parameter = Error(os);
        parameter.Write(file, line, message);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with Error object information,
      ///         error information in case of failure.
      static Writer_Type& Build(Writer_Type& writer, String_Type& file, int line, String_Type& message)
      {
        Write(writer, file, line, message);

        return writer;
      }

    private:
      Error(std::ostream& os) : stream(os), writer(this->stream) {}
      Error operator=(Error&) {}                                    // Not Implemented

      bool Write(String_Type& file, int line, String_Type& message)
      { return Write(this->writer, file, line, message); }

      static bool Write(Writer_Type& writer, String_Type& file, int line, String_Type& message)
      {
        writer.StartObject();
        writer.Key("type");
        writer.String("error");
        writer.Key("file");
        writer.String(file);
        writer.Key("line");
        writer.Int(line);
        writer.Key("message");
        writer.String("Error: " + message);
        writer.EndObject();

        return true;
      }

      Stream_Type stream; // Stream wrapper
      Writer_Type writer; // Writer used to fill the stream
  };
};

#endif() // MODULE_LOGGER_ERROR_HXX
