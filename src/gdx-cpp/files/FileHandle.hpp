
/*
    Copyright 2011 Aevum Software aevum @ aevumlab.com

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    @author Victor Vicente de Carvalho victor.carvalho@aevumlab.com
    @author Ozires Bortolon de Faria ozires@aevumlab.com
*/

#ifndef GDX_CPP_FILES_FILEHANDLE_HPP_
#define GDX_CPP_FILES_FILEHANDLE_HPP_

#include <iosfwd>
#include <string>
#include <vector>

#include "gdx-cpp/Files.hpp"

namespace gdx_cpp {

class File;

namespace files {

class FileHandle {
public:
    std::string& path ();
    std::string& name ();
    std::string& extension ();
    std::string& nameWithoutExtension ();
    gdx_cpp::Files::FileType& type ();
    std::istream& read ();
    std::string& readString ();
    std::string& readString (const std::string& charset);
    void readBytes (std::vector<char>& buffer);
    std::ostream& write (bool append);
    FileHandle* list ();
    FileHandle* list (const std::string& suffix);
    bool isDirectory ();
    FileHandle& child (const std::string& name);
    FileHandle& parent ();
    void mkdirs ();
    bool exists ();
    bool deleteFile ();
    bool deleteDirectory ();
    void copyTo (const FileHandle& dest);
    void moveTo (const FileHandle& dest);
    long length ();
    std::string& toString ();

protected:


private:
    File& file ();
    bool deleteDirectory (const File& file);
};

} // namespace gdx_cpp
} // namespace files

#endif // GDX_CPP_FILES_FILEHANDLE_HPP_