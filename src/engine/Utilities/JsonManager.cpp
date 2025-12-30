#include "Utilities/JsonManager.h"
#include "Utilities/functions.h"
#include "DrawSystem/RenderData/RenderData.h"
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include "MapManager/Chunk/Chunk.h"
#include "MapManager/MapManager.h"
#include "MapManager/Chunk/Block/Block.h"



//bool JsonManager::SaveToJson(RenderData_Particle& data, const std::string& path)
//{
//    try
//    {
//        json j;
//        if (data.name.has_value()) j["name"] = *data.name;
//        j["filePath"] = data.filePath;
//        j["texture"] = data.GetParticleInf().resource.texture;
//        j["model"] = data.GetParticleInf().resource.model;
//
//
//        // emitter
//        j["useSphereEmitter"] = data.GetParticleInf().emitter.useSphereEmitter;
//        j["emitFromInside"] = data.GetParticleInf().emitter.emitFromInside;
//        j["emitterAABB"]["min"] = ToJson(data.GetParticleInf().emitter.emitterAABB.min);
//        j["emitterAABB"]["max"] = ToJson(data.GetParticleInf().emitter.emitterAABB.max);
//        j["emitterSphere"]["center"] = ToJson(data.GetParticleInf().emitter.emitterSphere.center);
//        j["emitterSphere"]["radius"] = ToJson(data.GetParticleInf().emitter.emitterSphere.radius);
//
//        // scale
//        j["scale"]["isRandom_value"] = data.GetParticleInf().scale.isRandom_value;
//        j["scale"]["value"] = ToJson(data.GetParticleInf().scale.value);
//        j["scale"]["randomRange_value"]["min"] = ToJson(data.GetParticleInf().scale.randomRange_value.min);
//        j["scale"]["randomRange_value"]["max"] = ToJson(data.GetParticleInf().scale.randomRange_value.max);
//        j["scale"]["isRandom_velocity"] = data.GetParticleInf().scale.isRandom_velocity;
//        j["scale"]["velocity"] = ToJson(data.GetParticleInf().scale.velocity);
//        j["scale"]["randomRange_velocity"]["min"] = ToJson(data.GetParticleInf().scale.randomRange_velocity.min);
//        j["scale"]["randomRange_velocity"]["max"] = ToJson(data.GetParticleInf().scale.randomRange_velocity.max);
//        j["scale"]["isRandom_acceleration"] = data.GetParticleInf().scale.isRandom_acceleration;
//        j["scale"]["acceleration"] = ToJson(data.GetParticleInf().scale.acceleration);
//        j["scale"]["randomRange_acceleration"]["min"] = ToJson(data.GetParticleInf().scale.randomRange_acceleration.min);
//        j["scale"]["randomRange_acceleration"]["max"] = ToJson(data.GetParticleInf().scale.randomRange_acceleration.max);
//        // rotate
//        j["rotate"]["isRandom_value"] = data.GetParticleInf().rotate.isRandom_value;
//        j["rotate"]["value"] = ToJson(data.GetParticleInf().rotate.value);
//        j["rotate"]["randomRange_value"]["min"] = ToJson(data.GetParticleInf().rotate.randomRange_value.min);
//        j["rotate"]["randomRange_value"]["max"] = ToJson(data.GetParticleInf().rotate.randomRange_value.max);
//        j["rotate"]["isRandom_velocity"] = data.GetParticleInf().rotate.isRandom_velocity;
//        j["rotate"]["velocity"] = ToJson(data.GetParticleInf().rotate.velocity);
//        j["rotate"]["randomRange_velocity"]["min"] = ToJson(data.GetParticleInf().rotate.randomRange_velocity.min);
//        j["rotate"]["randomRange_velocity"]["max"] = ToJson(data.GetParticleInf().rotate.randomRange_velocity.max);
//        j["rotate"]["isRandom_acceleration"] = data.GetParticleInf().rotate.isRandom_acceleration;
//        j["rotate"]["acceleration"] = ToJson(data.GetParticleInf().rotate.acceleration);
//        j["rotate"]["randomRange_acceleration"]["min"] = ToJson(data.GetParticleInf().rotate.randomRange_acceleration.min);
//        j["rotate"]["randomRange_acceleration"]["max"] = ToJson(data.GetParticleInf().rotate.randomRange_acceleration.max);
//        // translate
//        j["translate"]["isRandom_value"] = data.GetParticleInf().translate.isRandom_value;
//        j["translate"]["value"] = ToJson(data.GetParticleInf().translate.value);
//        j["translate"]["randomRange_value"]["min"] = ToJson(data.GetParticleInf().translate.randomRange_value.min);
//        j["translate"]["randomRange_value"]["max"] = ToJson(data.GetParticleInf().translate.randomRange_value.max);
//        j["translate"]["isRandom_velocity"] = data.GetParticleInf().translate.isRandom_velocity;
//        j["translate"]["velocity"] = ToJson(data.GetParticleInf().translate.velocity);
//        j["translate"]["randomRange_velocity"]["min"] = ToJson(data.GetParticleInf().translate.randomRange_velocity.min);
//        j["translate"]["randomRange_velocity"]["max"] = ToJson(data.GetParticleInf().translate.randomRange_velocity.max);
//        j["translate"]["isRandom_acceleration"] = data.GetParticleInf().translate.isRandom_acceleration;
//        j["translate"]["acceleration"] = ToJson(data.GetParticleInf().translate.acceleration);
//        j["translate"]["randomRange_acceleration"]["min"] = ToJson(data.GetParticleInf().translate.randomRange_acceleration.min);
//        j["translate"]["randomRange_acceleration"]["max"] = ToJson(data.GetParticleInf().translate.randomRange_acceleration.max);
//
//        // target
//        j["target"] = ToJson(data.GetParticleInf().target.target);
//        j["spreadAngle"] = data.GetParticleInf().target.spreadAngle;
//        j["useTarget"] = data.GetParticleInf().target.useTarget;
//        j["spawnDependent"] = data.GetParticleInf().target.spawnDependent;
//        j["speed"] = data.GetParticleInf().target.speed;
//
//        // color
//        Vector4 vc = ConvertUintToVector4(data.GetParticleInf().material.color);
//        j["color"] = ToJson(vc);
//
//        // density
//        j["particlesPerEmission"] = data.GetParticleInf().density.particlesPerEmission;
//        j["emissionDelay"] = data.GetParticleInf().density.emissionDelay;
//        j["liveMax"] = data.GetParticleInf().density.liveMax;
//
//        // billboard
//        j["isBillboard"] = data.GetParticleInf().option.isBillboard;
//
//
//        // 最終出力パスは読み込みと同じく .json を付与
//        const std::string finalPath = path.ends_with(".json") ? path : (path + ".json");
//
//        // 親ディレクトリ作成
//        std::filesystem::path dst(finalPath);
//        if (!dst.parent_path().empty())
//        {
//            std::error_code ec;
//            std::filesystem::create_directories(dst.parent_path(), ec);
//            if (ec)
//            {
//                std::cerr << "Create directories failed: " << ec.message() << "\n";
//                return false;
//            }
//        }
//
//        // 一意な一時ファイル名を作る
//        const std::string tmp = finalPath + ".tmp." + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
//        {
//            std::ofstream ofs(tmp); // テキストなので binary は不要
//            if (!ofs)
//            {
//                std::cerr << "Failed to open tmp file for write: " << tmp << "\n";
//                return false;
//            }
//            ofs << j.dump(2);
//            if (!ofs.good())
//            {
//                std::cerr << "Failed to write JSON to tmp file\n";
//                std::filesystem::remove(tmp);
//                return false;
//            }
//        }
//
//        std::error_code ec;
//        std::filesystem::rename(tmp, finalPath, ec);
//        if (ec)
//        {
//            // 別ファイルシステム（EXDEV）ならコピー→削除で代替
//            if (std::filesystem::exists(finalPath))
//            {
//                std::filesystem::remove(finalPath, ec);
//                if (ec) { std::cerr << "remove existing failed: " << ec.message() << "\n"; std::filesystem::remove(tmp); return false; }
//            }
//
//            std::filesystem::rename(tmp, finalPath, ec);
//            if (ec) { std::cerr << "rename failed: " << ec.message() << "\n"; std::filesystem::remove(tmp); return false; }
//        }
//
//        return true;
//
//    }
//    catch (const std::exception& ex)
//    {
//        std::cerr << "SaveToJson exception: " << ex.what() << "\n";
//        return false;
//    }
//}
//
//bool JsonManager::LoadFromJson(RenderData_Particle& data, const std::string& path)
//{
//    const std::string filePathStr = path.ends_with(".json") ? path : (path + ".json");
//    std::filesystem::path filePath(filePathStr);
//
//    try
//    {
//        if (!std::filesystem::exists(filePath))
//        {
//            std::cerr << "LoadFromJson: file does not exist: " << filePathStr << "\n";
//            return false;
//        }
//
//        std::ifstream ifs;
//        try
//        {
//            ifs.open(filePathStr);
//            if (!ifs) { std::cerr << "LoadFromJson: failed to open file: " << filePathStr << "\n"; return false; }
//        }
//        catch (const std::exception& ex)
//        {
//            std::cerr << "LoadFromJson: exception opening file: " << ex.what() << " path=" << filePathStr << "\n";
//            return false;
//        }
//
//        json j;
//        try
//        {
//            ifs >> j;
//        }
//        catch (const std::exception& ex)
//        {
//            std::cerr << "LoadFromJson: parse JSON failed: " << ex.what() << " path=" << filePathStr << "\n";
//            return false;
//        }
//
//        // name
//        if (j.contains("name") && !j["name"].is_null() && j["name"].is_string())
//        {
//            data.name = j["name"].get<std::string>();
//        }
//
//        // filePath
//        if (j.contains("filePath") && j["filePath"].is_string())
//        {
//            data.filePath = j["filePath"].get<std::string>();
//        }
//
//        // texture
//        if (j.contains("texture") && !j["texture"].is_null())
//        {
//            try { data.GetParticleInf().resource.texture = j["texture"].get<decltype(data.GetParticleInf().resource.texture)>(); }
//            catch (...) { /* ignore invalid type */ }
//        }
//
//        // model
//        if (j.contains("model") && !j["model"].is_null())
//        {
//            try { data.GetParticleInf().resource.model = j["model"].get<decltype(data.GetParticleInf().resource.model)>(); }
//            catch (...) { /* ignore invalid type */ }
//        }
//
//        // emitter booleans
//        data.GetParticleInf().emitter.useSphereEmitter = j.value("useSphereEmitter", data.GetParticleInf().emitter.useSphereEmitter);
//        data.GetParticleInf().emitter.emitFromInside = j.value("emitFromInside", data.GetParticleInf().emitter.emitFromInside);
//
//        // emitterAABB
//        if (j.contains("emitterAABB") && j["emitterAABB"].is_object())
//        {
//            const auto& ea = j["emitterAABB"];
//            if (ea.contains("min") && ea["min"].is_array()) data.GetParticleInf().emitter.emitterAABB.min = ToVector3(ea["min"]);
//            if (ea.contains("max") && ea["max"].is_array()) data.GetParticleInf().emitter.emitterAABB.max = ToVector3(ea["max"]);
//        }
//
//        // emitterSphere
//        if (j.contains("emitterSphere") && j["emitterSphere"].is_object())
//        {
//            const auto& es = j["emitterSphere"];
//            if (es.contains("center") && es["center"].is_array()) data.GetParticleInf().emitter.emitterSphere.center = ToVector3(es["center"]);
//            if (es.contains("radius") && es["radius"].is_array()) data.GetParticleInf().emitter.emitterSphere.radius = ToVector3(es["radius"]);
//        }
//
//
//        // scale
//        if (j.contains("scale") && j["scale"].is_object())
//        {
//            const auto& s = j["scale"];
//            // flags
//            if (s.contains("isRandom_value")) data.GetParticleInf().scale.isRandom_value = s["isRandom_value"].get<bool>();
//            if (s.contains("isRandom_velocity")) data.GetParticleInf().scale.isRandom_velocity = s["isRandom_velocity"].get<bool>();
//            if (s.contains("isRandom_acceleration")) data.GetParticleInf().scale.isRandom_acceleration = s["isRandom_acceleration"].get<bool>();
//
//            // values
//            if (s.contains("value") && s["value"].is_array()) data.GetParticleInf().scale.value = ToVector3(s["value"]);
//            if (s.contains("velocity") && s["velocity"].is_array()) data.GetParticleInf().scale.velocity = ToVector3(s["velocity"]);
//            if (s.contains("acceleration") && s["acceleration"].is_array()) data.GetParticleInf().scale.acceleration = ToVector3(s["acceleration"]);
//
//            // random ranges
//            if (s.contains("randomRange_value") && s["randomRange_value"].is_object())
//            {
//                const auto& rr = s["randomRange_value"];
//                if (rr.contains("min") && rr["min"].is_array()) data.GetParticleInf().scale.randomRange_value.min = ToVector3(rr["min"]);
//                if (rr.contains("max") && rr["max"].is_array()) data.GetParticleInf().scale.randomRange_value.max = ToVector3(rr["max"]);
//            }
//            if (s.contains("randomRange_velocity") && s["randomRange_velocity"].is_object())
//            {
//                const auto& rr = s["randomRange_velocity"];
//                if (rr.contains("min") && rr["min"].is_array()) data.GetParticleInf().scale.randomRange_velocity.min = ToVector3(rr["min"]);
//                if (rr.contains("max") && rr["max"].is_array()) data.GetParticleInf().scale.randomRange_velocity.max = ToVector3(rr["max"]);
//            }
//            if (s.contains("randomRange_acceleration") && s["randomRange_acceleration"].is_object())
//            {
//                const auto& rr = s["randomRange_acceleration"];
//                if (rr.contains("min") && rr["min"].is_array()) data.GetParticleInf().scale.randomRange_acceleration.min = ToVector3(rr["min"]);
//                if (rr.contains("max") && rr["max"].is_array()) data.GetParticleInf().scale.randomRange_acceleration.max = ToVector3(rr["max"]);
//            }
//        }
//
//        // rotate
//        if (j.contains("rotate") && j["rotate"].is_object())
//        {
//            const auto& r = j["rotate"];
//            // flags
//            if (r.contains("isRandom_value")) data.GetParticleInf().rotate.isRandom_value = r["isRandom_value"].get<bool>();
//            if (r.contains("isRandom_velocity")) data.GetParticleInf().rotate.isRandom_velocity = r["isRandom_velocity"].get<bool>();
//            if (r.contains("isRandom_acceleration")) data.GetParticleInf().rotate.isRandom_acceleration = r["isRandom_acceleration"].get<bool>();
//
//            // values
//            if (r.contains("value") && r["value"].is_array()) data.GetParticleInf().rotate.value = ToVector3(r["value"]);
//            if (r.contains("velocity") && r["velocity"].is_array()) data.GetParticleInf().rotate.velocity = ToVector3(r["velocity"]);
//            if (r.contains("acceleration") && r["acceleration"].is_array()) data.GetParticleInf().rotate.acceleration = ToVector3(r["acceleration"]);
//
//            // random ranges
//            if (r.contains("randomRange_value") && r["randomRange_value"].is_object())
//            {
//                const auto& rr = r["randomRange_value"];
//                if (rr.contains("min") && rr["min"].is_array()) data.GetParticleInf().rotate.randomRange_value.min = ToVector3(rr["min"]);
//                if (rr.contains("max") && rr["max"].is_array()) data.GetParticleInf().rotate.randomRange_value.max = ToVector3(rr["max"]);
//            }
//            if (r.contains("randomRange_velocity") && r["randomRange_velocity"].is_object())
//            {
//                const auto& rr = r["randomRange_velocity"];
//                if (rr.contains("min") && rr["min"].is_array()) data.GetParticleInf().rotate.randomRange_velocity.min = ToVector3(rr["min"]);
//                if (rr.contains("max") && rr["max"].is_array()) data.GetParticleInf().rotate.randomRange_velocity.max = ToVector3(rr["max"]);
//            }
//            if (r.contains("randomRange_acceleration") && r["randomRange_acceleration"].is_object())
//            {
//                const auto& rr = r["randomRange_acceleration"];
//                if (rr.contains("min") && rr["min"].is_array()) data.GetParticleInf().rotate.randomRange_acceleration.min = ToVector3(rr["min"]);
//                if (rr.contains("max") && rr["max"].is_array()) data.GetParticleInf().rotate.randomRange_acceleration.max = ToVector3(rr["max"]);
//            }
//        }
//
//        // translate
//        if (j.contains("translate") && j["translate"].is_object())
//        {
//            const auto& t = j["translate"];
//            // flags
//            if (t.contains("isRandom_value")) data.GetParticleInf().translate.isRandom_value = t["isRandom_value"].get<bool>();
//            if (t.contains("isRandom_velocity")) data.GetParticleInf().translate.isRandom_velocity = t["isRandom_velocity"].get<bool>();
//            if (t.contains("isRandom_acceleration")) data.GetParticleInf().translate.isRandom_acceleration = t["isRandom_acceleration"].get<bool>();
//
//            // values
//            if (t.contains("value") && t["value"].is_array()) data.GetParticleInf().translate.value = ToVector3(t["value"]);
//            if (t.contains("velocity") && t["velocity"].is_array()) data.GetParticleInf().translate.velocity = ToVector3(t["velocity"]);
//            if (t.contains("acceleration") && t["acceleration"].is_array()) data.GetParticleInf().translate.acceleration = ToVector3(t["acceleration"]);
//
//            // random ranges
//            if (t.contains("randomRange_value") && t["randomRange_value"].is_object())
//            {
//                const auto& rr = t["randomRange_value"];
//                if (rr.contains("min") && rr["min"].is_array()) data.GetParticleInf().translate.randomRange_value.min = ToVector3(rr["min"]);
//                if (rr.contains("max") && rr["max"].is_array()) data.GetParticleInf().translate.randomRange_value.max = ToVector3(rr["max"]);
//            }
//            if (t.contains("randomRange_velocity") && t["randomRange_velocity"].is_object())
//            {
//                const auto& rr = t["randomRange_velocity"];
//                if (rr.contains("min") && rr["min"].is_array()) data.GetParticleInf().translate.randomRange_velocity.min = ToVector3(rr["min"]);
//                if (rr.contains("max") && rr["max"].is_array()) data.GetParticleInf().translate.randomRange_velocity.max = ToVector3(rr["max"]);
//            }
//            if (t.contains("randomRange_acceleration") && t["randomRange_acceleration"].is_object())
//            {
//                const auto& rr = t["randomRange_acceleration"];
//                if (rr.contains("min") && rr["min"].is_array()) data.GetParticleInf().translate.randomRange_acceleration.min = ToVector3(rr["min"]);
//                if (rr.contains("max") && rr["max"].is_array()) data.GetParticleInf().translate.randomRange_acceleration.max = ToVector3(rr["max"]);
//            }
//        }
//
//        // target
//        if (j.contains("target") && j["target"].is_array())
//        {
//            data.GetParticleInf().target.target = ToVector3(j["target"]);
//        }
//
//        // spreadAngle
//        data.GetParticleInf().target.spreadAngle = j.value("spreadAngle", data.GetParticleInf().target.spreadAngle);
//
//        // useTarget
//        data.GetParticleInf().target.useTarget = j.value("useTarget", data.GetParticleInf().target.useTarget);
//
//        // speed
//        data.GetParticleInf().target.speed = j.value("speed", data.GetParticleInf().target.speed);
//
//        data.GetParticleInf().target.spawnDependent = j.value("spawnDependent", data.GetParticleInf().target.spawnDependent);
//
//        // color
//        if (j.contains("color") && j["color"].is_array())
//        {
//			Vector4 vc = ToVector4(j["color"]);
//            data.GetParticleInf().material.color = ConvertVector4ToUint(vc);
//        }
//
//        // density / counts
//        data.GetParticleInf().density.particlesPerEmission = j.value("particlesPerEmission", data.GetParticleInf().density.particlesPerEmission);
//        data.GetParticleInf().density.emissionDelay = j.value("emissionDelay", data.GetParticleInf().density.emissionDelay);
//        data.GetParticleInf().density.liveMax = j.value("liveMax", data.GetParticleInf().density.liveMax);
//
//        // billboard
//        data.GetParticleInf().option.isBillboard = j.value("isBillboard", data.GetParticleInf().option.isBillboard);
//
//        return true;
//    }
//    catch (const std::exception& ex)
//    {
//        std::cerr << "LoadFromJson: exception: " << ex.what() << " path=" << filePathStr << "\n";
//        return false;
//    }
//    catch (...)
//    {
//        std::cerr << "LoadFromJson: unknown exception path=" << filePathStr << "\n";
//        return false;
//    }
//}

bool JsonManager::SaveToJson(RenderData_Particle& data, const std::string& path)
{
    return false;
}

bool JsonManager::LoadFromJson(RenderData_Particle& data, const std::string& path)
{
    return false;
}

bool JsonManager::SaveToJson(RenderData_Model& data, const std::string& path)
{
    try
    {
        json j;
        if (data.name.has_value()) j["name"] = *data.name;
        j["filePath"] = data.filePath;
		j["texture"] = data.GetTexture();
		j["model"] = data.GetModel();


        // scale
        j["scale"]["value"] = ToJson(data.scale.value);
        j["scale"]["velocity"] = ToJson(data.scale.velocity);
        j["scale"]["acceleration"] = ToJson(data.scale.acceleration);
        // rotate
        j["rotate"]["value"] = ToJson(data.rotate.value);
        j["rotate"]["velocity"] = ToJson(data.rotate.velocity);
        j["rotate"]["acceleration"] = ToJson(data.rotate.acceleration);
        // translate
        j["translate"]["value"] = ToJson(data.translate.value);
        j["translate"]["velocity"] = ToJson(data.translate.velocity);
        j["translate"]["acceleration"] = ToJson(data.translate.acceleration);

        // color
        j["color"] = ToJson(data.color);

        // 出力パスに.json を付与
        const std::string finalPath = path.ends_with(".json") ? path : (path + ".json");

        // 親ディレクトリ作成
        std::filesystem::path dst(finalPath);
        if (!dst.parent_path().empty())
        {
            std::error_code ec;
            std::filesystem::create_directories(dst.parent_path(), ec);
            if (ec)
            {
                std::cerr << "Create directories failed: " << ec.message() << "\n";
                return false;
            }
        }

        // 一意な一時ファイル名を作る
        const std::string tmp = finalPath + ".tmp." + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        {
            std::ofstream ofs(tmp); // テキストなので binary は不要
            if (!ofs)
            {
                std::cerr << "Failed to open tmp file for write: " << tmp << "\n";
                return false;
            }
            ofs << j.dump(2);
            if (!ofs.good())
            {
                std::cerr << "Failed to write JSON to tmp file\n";
                std::filesystem::remove(tmp);
                return false;
            }
        }

        std::error_code ec;
        std::filesystem::rename(tmp, finalPath, ec);
        if (ec)
        {
            // 別ファイルシステム（EXDEV）ならコピー→削除で代替
            if (std::filesystem::exists(finalPath))
            {
                std::filesystem::remove(finalPath, ec);
                if (ec) { std::cerr << "remove existing failed: " << ec.message() << "\n"; std::filesystem::remove(tmp); return false; }
            }

            std::filesystem::rename(tmp, finalPath, ec);
            if (ec) { std::cerr << "rename failed: " << ec.message() << "\n"; std::filesystem::remove(tmp); return false; }
        }

        return true;

    }
    catch (const std::exception& ex)
    {
        std::cerr << "SaveToJson exception: " << ex.what() << "\n";
        return false;
    }

}

bool JsonManager::LoadFromJson(RenderData_Model& m, const std::string& path)
{
    return false;
}

bool JsonManager::SaveToJson(MapManager& data, const std::string& path)
{
    try
    {
        json root;
        json mapData = json::array();

        // 全チャンクをループ
        for (const auto& [chunkPos, chunkPtr] : data.chunks)
        {
            Chunk& chunk = *chunkPtr;

            // チャンクキー "[x][y]"
            std::string chunkKey = "[" + std::to_string(chunk.chunkPos.x) + "][" + std::to_string(chunk.chunkPos.y) + "]";

            json chunkEntry;          // { "[5][5]": [...] }
            json blockList = json::array();  // BlockID1, BlockID2, ...

            // BlockID ごとにまとめる
            for (int32_t i = 1; i < int32_t(BlockID::MAX); ++i)
            {
                BlockID id = BlockID(i);
                RenderData_Block* blockData = chunk.blockData_[id].get();
                if (!blockData) continue;
               
                json blockEntry;  // { "BlockID1": [ {...}, {...} ] }
                std::string blockKey = "BlockID" + std::to_string(i);

                json positions = json::array();

                for (const auto& pos : data.chunks[chunk.chunkPos]->blockPositions[id])
                {
                    json posObj;
                    posObj["position"] = ToJson(pos);
					positions.push_back(posObj);
                }


                //for (uint32_t idx = 0; idx < blockData->capacity; ++idx)
                //{
                //    if (blockData->isActive_[idx])
                //    {
                //        json posObj;
                //        posObj["position"] = ToJson(blockData->indexes_[idx]);
                //        positions.push_back(posObj);
                //    }
                //}

                blockEntry[blockKey] = positions;
                blockList.push_back(blockEntry);
            }

            chunkEntry[chunkKey] = blockList;
            mapData.push_back(chunkEntry);
        }

        root["mapData"] = mapData;

        // 保存
        std::ofstream ofs(path);
        ofs << root.dump(2);
        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "SaveWholeMap exception: " << ex.what() << "\n";
        return false;
    }
}

bool JsonManager::LoadFromJson(MapManager& data, const std::string& path)
{
    try
    {
        std::ifstream ifs(path);
        if (!ifs)
        {
            std::cerr << "Failed to open map file: " << path << "\n";
            return false;
        }

        json root;
        ifs >> root;

        if (!root.contains("mapData"))
        {
            std::cerr << "Invalid map file: missing mapData\n";
            return false;
        }

        for (const auto& chunkEntry : root["mapData"])
        {
            // chunkEntry は { "[x][y]": [...] } の形
            for (auto it = chunkEntry.begin(); it != chunkEntry.end(); ++it)
            {
                std::string chunkKey = it.key(); // "[5][5]" など

                // チャンク座標をパース
				Vector2int chunkPos;
				// "[5][5]" -> 5, 5
				size_t firstBracketClose = chunkKey.find(']');
				size_t secondBracketOpen = chunkKey.find('[', firstBracketClose);
				chunkPos.x = std::stoi(chunkKey.substr(1, firstBracketClose - 1));
				chunkPos.y = std::stoi(chunkKey.substr(secondBracketOpen + 1, chunkKey.find(']', secondBracketOpen) - secondBracketOpen - 1));

				// チャンクを作成（まだブロックのインスタンスは作成しない）
                Chunk* chunk = new Chunk();
				chunk->loadResult = true;   // 読み込みフラグ
				chunk->chunkPos = chunkPos; // チャンク座標設定

                // BlockID の配列を取得
                const json& blockList = it.value();

                for (const auto& blockEntry : blockList)
                {
                    // blockEntry は { "BlockID1": [...] } の形
                    for (auto bit = blockEntry.begin(); bit != blockEntry.end(); ++bit)
                    {
                        std::string blockKey = bit.key(); // "BlockID1"
                        int blockID = std::stoi(blockKey.substr(7)); // "1" を取り出す

                        const json& positions = bit.value();

                        for (const auto& posObj : positions)
                        {
							Vector3int pos = ToVector3int(posObj["position"]);  // ブロック位置獲得

							chunk->blockPositions[BlockID(blockID)].push_back(pos);
                        }
                    }
                }

				data.chunks[chunkPos] = chunk;
            }
        }


        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "LoadWholeMap exception: " << ex.what() << "\n";
        return false;
    }

}



bool JsonManager::SaveToJson(const std::string& path, const std::string key, const int& data)
{
    return false;
}
bool JsonManager::SaveToJson(const std::string& path, const std::string key, const float& data)
{
    try
    {
#pragma region jsonファイルとディレクトリの存在確認と作成

		std::string savePath;           // A/B/C.json
		bool isExist = false;           // A/B/C.jsonがあるか
        std::string directoryPath;      // A/B
		bool isDirectoryExist = false;  // A/Bファイルがあるか
        
        savePath = path.ends_with(".json") ? path : (path + ".json");
		directoryPath = std::filesystem::path(savePath).parent_path().string();

        if (std::filesystem::exists(savePath))isExist = true;
		if (std::filesystem::exists(directoryPath))isDirectoryExist = true;
        else
        {
            std::error_code ec;
            std::filesystem::create_directories(directoryPath, ec);
            if (ec)
            {
                std::cerr << "Create directories failed: " << ec.message() << "\n";
                return false;
			}
        }

#pragma endregion

#pragma region JSONデータの読み込み

		json newJson;       // 保存するデータ
		json existingJson;  // 既存データ

        // newJsonにdataを追加
		newJson = SetJsonValue(key, data);

		// existingJsonに既存データを読み込む 
        if (isExist)
        {
            std::ifstream ifs(savePath);
            if (ifs)
            {
                try
                {
                    ifs >> existingJson;
                }
                catch (const std::exception& ex)
                {
                    std::cerr << "SaveToJson: parse existing JSON failed: " << ex.what() << " path=" << savePath << "\n";
                    // 既存ファイルが壊れている場合は無視して新規作成に進む
                }
            }
        }

#pragma endregion

#pragma region 新規データと既存データのマージ

        for (auto& [k, v] : existingJson.items())
        {
            // newJsonに存在しないキーは既存データを引き継ぐ
            if (!newJson.contains(k))
            {
                newJson[k] = v;
            }
        }

#pragma endregion
        
#pragma region マージしたデータを保存

		// 保存失敗時にすべtえのデータが破壊されるのを防ぐため、一時ファイル経由で保存する
        const std::string tmp = savePath + ".tmp." + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        {
            std::ofstream ofs(tmp);
            if (!ofs)
            {
                std::cerr << "Failed to open tmp file for write: " << tmp << "\n";
                return false;
            }
            ofs << newJson.dump(2);
            if (!ofs.good())
            {
                std::cerr << "Failed to write JSON to tmp file\n";
                std::filesystem::remove(tmp);
                return false;
            }
        }
        std::error_code ec;
        std::filesystem::rename(tmp, savePath, ec);
        if (ec)
        {
            // 別ファイルシステム（EXDEV）ならコピー→削除で代替
            if (std::filesystem::exists(savePath))
            {
                std::filesystem::remove(savePath, ec);
                if (ec) { std::cerr << "remove existing failed: " << ec.message() << "\n"; std::filesystem::remove(tmp); return false; }
            }
            std::filesystem::rename(tmp, savePath, ec);
            if (ec) { std::cerr << "rename failed: " << ec.message() << "\n"; std::filesystem::remove(tmp); return false; }
        }
		return true;

#pragma endregion

    }
    catch (const std::exception& ex)
    {
        std::cerr << "SaveToJson exception: " << ex.what() << "\n";
        return false;
    }

    return false;
}
bool JsonManager::SaveToJson(const std::string& path, const std::string key, const std::string& data)
{
    return false;
}
bool JsonManager::SaveToJson(const std::string& path, const std::string key, const Vector2int& data)
{
    return false;
}
bool JsonManager::SaveToJson(const std::string& path, const std::string key, const Vector2& data)
{
    return false;
}
bool JsonManager::SaveToJson(const std::string& path, const std::string key, const Vector3& data)
{
    return false;
}
bool JsonManager::SaveToJson(const std::string& path, const std::string key, const Vector4& data)
{
    return false;
}
bool JsonManager::SaveToJson(const std::string& path, const std::string key, const AABB& data)
{
    return false;
}

json JsonManager::ToJson(const int& data)
{
    return data;
}
json JsonManager::ToJson(const float& data)
{
	return data;
}
json JsonManager::ToJson(const std::string& data)
{
	return data;
}
json JsonManager::ToJson(const Vector2int& data)
{
	return json::array({ data.x, data.y });
}
json JsonManager::ToJson(const Vector2& data)
{
	return json::array({ data.x, data.y });
}
json JsonManager::ToJson(const Vector3& data)
{
    return json::array({ data.x, data.y, data.z });
}
json JsonManager::ToJson(const Vector3int& data)
{
	return json::array({ data.x, data.y, data.z });
}
json JsonManager::ToJson(const Vector4& data)
{
    return json::array({ data.x, data.y, data.z, data.w });
}
json JsonManager::ToJson(const AABB& data)
{
    return json::object({
        {"min", ToJson(data.min)},
        {"max", ToJson(data.max)}
		});
}
json JsonManager::ToJson(const Matrix3x3& data)
{
    return json::array({
          data.m[0][0], data.m[0][1], data.m[0][2],
          data.m[1][0], data.m[1][1], data.m[1][2],
          data.m[2][0], data.m[2][1], data.m[2][2]
		});
}
json JsonManager::ToJson(const Matrix4x4& data)
{
    return json::array({
          data.m[0][0], data.m[0][1], data.m[0][2], data.m[0][3],
          data.m[1][0], data.m[1][1], data.m[1][2], data.m[1][3],
          data.m[2][0], data.m[2][1], data.m[2][2], data.m[2][3],
          data.m[3][0], data.m[3][1], data.m[3][2], data.m[3][3]
        });
}

int JsonManager::ToInt(const json& j)
{
	return j.get<int>();
}
float JsonManager::ToFloat(const json& j)
{
	return j.get<float>();
}
std::string JsonManager::ToString(const json& j)
{
	return j.get<std::string>();
}
Vector2int JsonManager::ToVector2int(const json& j)
{
    if (!j.is_array() || j.size() < 2) return Vector2int{};
	return Vector2int{ j[0].get<int>(), j[1].get<int>() };
}
Vector2 JsonManager::ToVector2(const json& j)
{
    if (!j.is_array() || j.size() < 2) return Vector2{};
	return Vector2{ j[0].get<float>(), j[1].get<float>() };
}
Vector3 JsonManager::ToVector3(const json& j)
{
    if (!j.is_array() || j.size() < 3) return Vector3{};
    return Vector3{ j[0].get<float>(), j[1].get<float>(), j[2].get<float>() };
}
Vector3int JsonManager::ToVector3int(const json& j)
{
    if (!j.is_array() || j.size() < 3) return Vector3int{};
	return Vector3int{ j[0].get<int>(), j[1].get<int>(), j[2].get<int>() };
}
Vector4 JsonManager::ToVector4(const json& j)
{
    if (!j.is_array() || j.size() < 4) return Vector4{};
    return Vector4{ j[0].get<float>(), j[1].get<float>(), j[2].get<float>(), j[3].get<float>() };
}
AABB JsonManager::ToAABB(const json& j)
{
    AABB result;
    if (j.contains("min") && j["min"].is_array()) result.min = ToVector3(j["min"]);
    if (j.contains("max") && j["max"].is_array()) result.max = ToVector3(j["max"]);
	return result;
}
Matrix3x3 JsonManager::ToMatrix3x3(const json& j)
{
	Matrix3x3 result;
    if (!j.is_array() || j.size() < 9) return result;
    result.m[0][0] = j[0].get<float>();
    result.m[0][1] = j[1].get<float>();
    result.m[0][2] = j[2].get<float>();
    result.m[1][0] = j[3].get<float>();
    result.m[1][1] = j[4].get<float>();
    result.m[1][2] = j[5].get<float>();
    result.m[2][0] = j[6].get<float>();
    result.m[2][1] = j[7].get<float>();
	result.m[2][2] = j[8].get<float>();
	return result;
}
Matrix4x4 JsonManager::ToMatrix4x4(const json& j)
{
	Matrix4x4 result;
    if (!j.is_array() || j.size() < 16) return result;
    result.m[0][0] = j[0].get<float>();
    result.m[0][1] = j[1].get<float>();
    result.m[0][2] = j[2].get<float>();
    result.m[0][3] = j[3].get<float>();
    result.m[1][0] = j[4].get<float>();
    result.m[1][1] = j[5].get<float>();
    result.m[1][2] = j[6].get<float>();
    result.m[1][3] = j[7].get<float>();
    result.m[2][0] = j[8].get<float>();
    result.m[2][1] = j[9].get<float>();
    result.m[2][2] = j[10].get<float>();
    result.m[2][3] = j[11].get<float>();
    result.m[3][0] = j[12].get<float>();
    result.m[3][1] = j[13].get<float>();
    result.m[3][2] = j[14].get<float>();
	result.m[3][3] = j[15].get<float>();
    return result;
}


json JsonManager::SetJsonValue(const std::string& key, const float& data)
{
    json result;

    std::vector<std::string> keys = SplitString(key, '/');

    json* current = &result;
    for (size_t i = 0; i < keys.size(); ++i)
    {
        const std::string& k = keys[i];
        if (i == keys.size() - 1)
        {
            // 最後のキーならデータをセット
            (*current)[k] = ToJson(data);
        }
        else
        {
            // 中間のキーならオブジェクトを作成して進む
            if (!current->contains(k) || !(*current)[k].is_object())
            {
                (*current)[k] = json::object();
            }
            current = &((*current)[k]);
        }
    }

    return result;
}
std::vector<std::string> JsonManager::SplitString(const std::string& str, char delimiter)
{
    std::vector<std::string> result;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter))
    {
        result.push_back(token);
    }
    return result;
}


