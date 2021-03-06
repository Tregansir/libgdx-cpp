
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

#ifndef GDX_CPP_ASSETS_LOADERS_BITMAPFONTLOADER_HPP_
#define GDX_CPP_ASSETS_LOADERS_BITMAPFONTLOADER_HPP_

#include "AssetLoader.hpp"
#include "gdx-cpp/graphics/g2d/BitmapFont.hpp"
#include "BitmapFontParameter.hpp"

namespace gdx {

    class AssetManager;

class BitmapFontLoader : public AssetLoader<BitmapFont, BitmapFontParameter> {
public:
    std::vector<AssetDescriptor> getDependencies (const std::string& fileName, const Parameter* parameter);
    void loadAsync (const AssetManager* manager, const std::string& fileName, const BitmapFontParameter& parameter);
    BitmapFont& loadSync ();

protected:
    BitmapFont::BitmapFontData::ptr data;
    AssetManager* manager;
    std::string fileName;
};

} // namespace gdx

#endif // GDX_CPP_ASSETS_LOADERS_BITMAPFONTLOADER_HPP_
