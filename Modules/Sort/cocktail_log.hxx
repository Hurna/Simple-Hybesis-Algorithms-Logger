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
#ifndef MODULE_SORT_COCKTAIL_LOG_HXX
#define MODULE_SORT_COCKTAIL_LOG_HXX

#include <Logger/algorithm.hxx>
#include <Logger/array.hxx>
#include <Logger/comment.hxx>
#include <Logger/operation.hxx>
#include <Logger/typedef.hxx>
#include <Logger/value.hxx>
#include <partition_log.hxx>

// STD includes
#include <iterator>

namespace SHA_Logger
{
  /// @class CocktailLog
  ///
  template <typename IT, typename Compare = std::less_equal<typename std::iterator_traits<IT>::value_type>>
  class CocktailLog
  {
    public:
      /// eg https://cs.chromium.org/chromium/src/gpu/config/software_rendering_list_json.cc
      static const String GetName() { return "Cocktail_Sort"; }

      /// Write algorithm information
      /// @todo Use string litteral for JSON description within c++ code
      static bool WriteInfo(Writer& writer) { return true; }

      /// Write algorithm decription
      /// @todo Use string litteral for JSON description within c++ code
      static bool WriteDoc(Writer& writer) { return true; }

      /// Write algorithm sources
      /// @todo Use string litteral for JSON description within c++ code
      static bool WriteSrc(Writer& writer) { return true; }

      // Assert correct JSON construction.
      ~CocktailLog() { assert(this->writer->IsComplete()); }

      /// Instantiate a new json writer using the stream passed as
      /// argument, run and write algorithm computation information.
      ///
      /// @return stream reference filled up with CocktailLog object information,
      ///         error object information in case of failure.
      static Ostream& Build(Ostream& os, Options opts, const IT& begin, const IT& end)
      {
        std::unique_ptr<CocktailLog> builder = std::unique_ptr<CocktailLog>(new CocktailLog(os));
        builder->Write(opts, begin, end);

        return os;
      }

      /// Use json writer passed as parameter to write iterator information.
      ///
      /// @return stream reference filled up with CocktailLog object information,
      ///         error information in case of failure.
      static Writer& Build(Writer& writer, Options opts, const IT& begin, const IT& end)
      {
        Write(writer, opts, begin, end);

        return writer;
      }

    private:
      CocktailLog(Ostream& os) : stream(std::unique_ptr<Stream>(new Stream(os))),
                               writer(std::unique_ptr<Writer>(new Writer(*this->stream))) {}
      CocktailLog operator=(CocktailLog&) {} // Not Implemented

      bool Write(Options opts, const IT& begin, const IT& end)
      { return Write(*this->writer, opts, begin, end); }

      static bool Write(Writer& writer, Options opts, const IT& begin, const IT& end)
      {
        // Do not write sequence if no data to be processed
        const int _seqSize = static_cast<int>(std::distance(begin, end));
        if (_seqSize < 2)
        {
          Comment::Build(writer, "Sequence size too small to be processed.", 0);
          Operation::Return<bool>(writer, true);
          return true;
        }

        writer.StartObject();

        // Write description
        Algo_Traits<CocktailLog>::Build(writer, opts);

        // Write parameters
        WriteParameters(writer, opts, begin, end);

        // Write computation
        WriteComputation(writer, begin, end);

        writer.EndObject();

        return true;
      }

      ///
      static bool WriteParameters(Writer& writer, Options opts, const IT& begin, const IT& end)
      {
        writer.Key("parameters");
        writer.StartArray();
        if (opts & OpIsSub)
        {
          const int _seqSize = static_cast<int>(std::distance(begin, end));
          Iterator::Build(writer, kSeqName, "begin", 0);
          Iterator::Build(writer, kSeqName, "end", _seqSize);
        }
        else
        {
          Array<IT>::Build(writer, kSeqName, "begin", begin, "end", end);
        }
        writer.EndArray();

        return true;
      }

      ///
      static bool WriteComputation(Writer& writer, const IT& begin, const IT& end)
      {
        // Local logged variables
        writer.Key("locals");
        writer.StartArray();
        auto it = Iterator::BuildIt<IT>(writer, kSeqName, "it", 0, begin);
        writer.EndArray();

        // Proceed sort
        writer.Key("logs");
        writer.StartArray();
        const int kdistance = static_cast<int>(std::distance(begin, end));
        int beginIdx = 0;
        int endIdx = kdistance - 1;
        bool hasSwapped = true;
        // for each element from beginning - bubble it up until the end.
        while (hasSwapped && beginIdx < kdistance - 1)
        {
          hasSwapped = false;
          int _itIdx = beginIdx;
          Operation::Set<int>(writer, "it", _itIdx);
          for (it = begin + beginIdx; it < begin + endIdx; ++it)
          {
            if (Compare()(*(it + 1), *it))
            {
              std::swap(*it, *(it + 1));
              Operation::Swap(writer, "it", "it++");
              hasSwapped = true;
            }
            Operation::Set<int>(writer, "it", ++_itIdx);
          }
          --endIdx;

          if (!hasSwapped)
            break;

          // for each element from the end- bubble it down until the beginning.
          _itIdx = endIdx - 1;
          Operation::Set<int>(writer, "it", _itIdx);
          for (it = begin + endIdx - 1; it >= begin + beginIdx; --it)
          {
            if (Compare()(*(it + 1), *it))
            {
              std::swap(*it, *(it + 1));
              Operation::Swap(writer, "it", "it++");
              hasSwapped = true;
            }
            Operation::Set<int>(writer, "it", --_itIdx);
          }
          ++beginIdx;
        }

        Operation::Return<bool>(writer, true);
        writer.EndArray();

        return true;
      }

      std::unique_ptr<Stream> stream; // Stream wrapper
      std::unique_ptr<Writer> writer; // Writer used to fill the stream
  };
}

#endif // MODULE_SORT_COCKTAIL_LOG_HXX
