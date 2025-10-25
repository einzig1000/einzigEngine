#include "Utilities/JsonManager.h"
#include "Utilities/functions.h"
#include "DrawSystem/RenderData/RenderData.h"
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>



bool JsonManager::SaveToJson(const RenderData_Particle& p, const std::string& path)
{
    try
    {
        json j;
        if (p.name.has_value()) j["name"] = *p.name;
        j["filePath"] = p.filePath;
        j["texture"] = p.texture;
        j["model"] = p.model;


        // emitter
        j["useSphereEmitter"] = p.useSphereEmitter;
        j["emitFromInside"] = p.emitFromInside;
        j["emitterAABB"]["min"] = Vec3ToJson(p.emitterAABB.min);
        j["emitterAABB"]["max"] = Vec3ToJson(p.emitterAABB.max);
        j["emitterSphere"]["center"] = Vec3ToJson(p.emitterSphere.center);
        j["emitterSphere"]["radius"] = Vec3ToJson(p.emitterSphere.radius);

        // scale
		j["scale"]["isRandom_value"] = p.scale.isRandom_value;
        j["scale"]["value"] = Vec3ToJson(p.scale.value);
        j["scale"]["randomRange_value"]["min"] = Vec3ToJson(p.scale.randomRange_value.min);
        j["scale"]["randomRange_value"]["max"] = Vec3ToJson(p.scale.randomRange_value.max);
		j["scale"]["isRandom_velocity"] = p.scale.isRandom_velocity;
        j["scale"]["velocity"] = Vec3ToJson(p.scale.velocity);
        j["scale"]["randomRange_velocity"]["min"] = Vec3ToJson(p.scale.randomRange_velocity.min);
        j["scale"]["randomRange_velocity"]["max"] = Vec3ToJson(p.scale.randomRange_velocity.max);
		j["scale"]["isRandom_acceleration"] = p.scale.isRandom_acceleration;
        j["scale"]["acceleration"] = Vec3ToJson(p.scale.acceleration);
        j["scale"]["randomRange_acceleration"]["min"] = Vec3ToJson(p.scale.randomRange_acceleration.min);
        j["scale"]["randomRange_acceleration"]["max"] = Vec3ToJson(p.scale.randomRange_acceleration.max);
        // rotate
		j["rotate"]["isRandom_value"] = p.rotate.isRandom_value;
        j["rotate"]["value"] = Vec3ToJson(p.rotate.value);
        j["rotate"]["randomRange_value"]["min"] = Vec3ToJson(p.rotate.randomRange_value.min);
        j["rotate"]["randomRange_value"]["max"] = Vec3ToJson(p.rotate.randomRange_value.max);
		j["rotate"]["isRandom_velocity"] = p.rotate.isRandom_velocity;
        j["rotate"]["velocity"] = Vec3ToJson(p.rotate.velocity);
        j["rotate"]["randomRange_velocity"]["min"] = Vec3ToJson(p.rotate.randomRange_velocity.min);
        j["rotate"]["randomRange_velocity"]["max"] = Vec3ToJson(p.rotate.randomRange_velocity.max);
		j["rotate"]["isRandom_acceleration"] = p.rotate.isRandom_acceleration;
        j["rotate"]["acceleration"] = Vec3ToJson(p.rotate.acceleration);
        j["rotate"]["randomRange_acceleration"]["min"] = Vec3ToJson(p.rotate.randomRange_acceleration.min);
        j["rotate"]["randomRange_acceleration"]["max"] = Vec3ToJson(p.rotate.randomRange_acceleration.max);
        // translate
		j["translate"]["isRandom_value"] = p.translate.isRandom_value;
        j["translate"]["value"] = Vec3ToJson(p.translate.value);
		j["translate"]["randomRange_value"]["min"] = Vec3ToJson(p.translate.randomRange_value.min);
		j["translate"]["randomRange_value"]["max"] = Vec3ToJson(p.translate.randomRange_value.max);
		j["translate"]["isRandom_velocity"] = p.translate.isRandom_velocity;
        j["translate"]["velocity"] = Vec3ToJson(p.translate.velocity);
		j["translate"]["randomRange_velocity"]["min"] = Vec3ToJson(p.translate.randomRange_velocity.min);
		j["translate"]["randomRange_velocity"]["max"] = Vec3ToJson(p.translate.randomRange_velocity.max);
		j["translate"]["isRandom_acceleration"] = p.translate.isRandom_acceleration;
        j["translate"]["acceleration"] = Vec3ToJson(p.translate.acceleration);
		j["translate"]["randomRange_acceleration"]["min"] = Vec3ToJson(p.translate.randomRange_acceleration.min);
		j["translate"]["randomRange_acceleration"]["max"] = Vec3ToJson(p.translate.randomRange_acceleration.max);

        // target
		j["target"] = Vec3ToJson(p.target);
		j["spreadAngle"] = p.spreadAngle;
		j["useTarget"] = p.useTarget;
        j["spawnDependent"] = p.spawnDependent;
		j["speed"] = p.speed;

		// color
        Vector4 vc = ConvertUintToVector4(p.color);
        j["color"] = Vec4ToJson(vc);

        // density
        j["particlesPerEmission"] = p.particlesPerEmission;
        j["emissionDelay"] = p.emissionDelay;
        j["liveMax"] = p.liveMax;

		// billboard
        j["isBillboard"] = p.isBillboard;


        // 最終出力パスは読み込みと同じく .json を付与
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

bool JsonManager::LoadFromJson(RenderData_Particle& data, const std::string& path)
{
        const std::string filePathStr = path.ends_with(".json") ? path : (path + ".json");
        std::filesystem::path filePath(filePathStr);

    try
    {
        if (!std::filesystem::exists(filePath))
        {
            std::cerr << "LoadFromJson: file does not exist: " << filePathStr << "\n";
            return false;
        }

        std::ifstream ifs;
        try
        {
            ifs.open(filePathStr);
            if (!ifs) { std::cerr << "LoadFromJson: failed to open file: " << filePathStr << "\n"; return false; }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "LoadFromJson: exception opening file: " << ex.what() << " path=" << filePathStr << "\n";
            return false;
        }

        json j;
        try
        {
            ifs >> j;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "LoadFromJson: parse JSON failed: " << ex.what() << " path=" << filePathStr << "\n";
            return false;
        }

        // name
        if (j.contains("name") && !j["name"].is_null() && j["name"].is_string())
        {
            data.name = j["name"].get<std::string>();
        }

        // filePath
        if (j.contains("filePath") && j["filePath"].is_string())
        {
            data.filePath = j["filePath"].get<std::string>();
        }

        // texture
        if (j.contains("texture") && !j["texture"].is_null())
        {
            try { data.texture = j["texture"].get<decltype(data.texture)>(); }
            catch (...) { /* ignore invalid type */ }
        }
        else if (j.contains("texture_index") && !j["texture_index"].is_null())
        {
            try { data.texture = j["texture_index"].get<decltype(data.texture)>(); }
            catch (...) { /* ignore invalid type */ }
        }

        // model
        if (j.contains("model") && !j["model"].is_null())
        {
            try { data.model = j["model"].get<decltype(data.model)>(); }
            catch (...) { /* ignore invalid type */ }
        }

        // emitter booleans
        data.useSphereEmitter = j.value("useSphereEmitter", data.useSphereEmitter);
        data.emitFromInside = j.value("emitFromInside", data.emitFromInside);

        // emitterAABB
        if (j.contains("emitterAABB") && j["emitterAABB"].is_object())
        {
            const auto& ea = j["emitterAABB"];
            if (ea.contains("min") && ea["min"].is_array()) data.emitterAABB.min = JsonToVec3(ea["min"], data.emitterAABB.min);
            if (ea.contains("max") && ea["max"].is_array()) data.emitterAABB.max = JsonToVec3(ea["max"], data.emitterAABB.max);
        }

        // emitterSphere
        if (j.contains("emitterSphere") && j["emitterSphere"].is_object())
        {
            const auto& es = j["emitterSphere"];
            if (es.contains("center") && es["center"].is_array()) data.emitterSphere.center = JsonToVec3(es["center"], data.emitterSphere.center);
            if (es.contains("radius") && es["radius"].is_array()) data.emitterSphere.radius = JsonToVec3(es["radius"], data.emitterSphere.radius);
        }


        // scale
        if (j.contains("scale") && j["scale"].is_object())
        {
            const auto& s = j["scale"];
            // flags
            if (s.contains("isRandom_value")) data.scale.isRandom_value = s["isRandom_value"].get<bool>();
            if (s.contains("isRandom_velocity")) data.scale.isRandom_velocity = s["isRandom_velocity"].get<bool>();
            if (s.contains("isRandom_acceleration")) data.scale.isRandom_acceleration = s["isRandom_acceleration"].get<bool>();

            // values
            if (s.contains("value") && s["value"].is_array()) data.scale.value = JsonToVec3(s["value"], data.scale.value);
            if (s.contains("velocity") && s["velocity"].is_array()) data.scale.velocity = JsonToVec3(s["velocity"], data.scale.velocity);
            if (s.contains("acceleration") && s["acceleration"].is_array()) data.scale.acceleration = JsonToVec3(s["acceleration"], data.scale.acceleration);

            // random ranges
            if (s.contains("randomRange_value") && s["randomRange_value"].is_object())
            {
                const auto& rr = s["randomRange_value"];
                if (rr.contains("min") && rr["min"].is_array()) data.scale.randomRange_value.min = JsonToVec3(rr["min"], data.scale.randomRange_value.min);
                if (rr.contains("max") && rr["max"].is_array()) data.scale.randomRange_value.max = JsonToVec3(rr["max"], data.scale.randomRange_value.max);
            }
            if (s.contains("randomRange_velocity") && s["randomRange_velocity"].is_object())
            {
                const auto& rr = s["randomRange_velocity"];
                if (rr.contains("min") && rr["min"].is_array()) data.scale.randomRange_velocity.min = JsonToVec3(rr["min"], data.scale.randomRange_velocity.min);
                if (rr.contains("max") && rr["max"].is_array()) data.scale.randomRange_velocity.max = JsonToVec3(rr["max"], data.scale.randomRange_velocity.max);
            }
            if (s.contains("randomRange_acceleration") && s["randomRange_acceleration"].is_object())
            {
                const auto& rr = s["randomRange_acceleration"];
                if (rr.contains("min") && rr["min"].is_array()) data.scale.randomRange_acceleration.min = JsonToVec3(rr["min"], data.scale.randomRange_acceleration.min);
                if (rr.contains("max") && rr["max"].is_array()) data.scale.randomRange_acceleration.max = JsonToVec3(rr["max"], data.scale.randomRange_acceleration.max);
            }
        }

        // rotate
        if (j.contains("rotate") && j["rotate"].is_object())
        {
            const auto& r = j["rotate"];
            // flags
            if (r.contains("isRandom_value")) data.rotate.isRandom_value = r["isRandom_value"].get<bool>();
            if (r.contains("isRandom_velocity")) data.rotate.isRandom_velocity = r["isRandom_velocity"].get<bool>();
            if (r.contains("isRandom_acceleration")) data.rotate.isRandom_acceleration = r["isRandom_acceleration"].get<bool>();

            // values
            if (r.contains("value") && r["value"].is_array()) data.rotate.value = JsonToVec3(r["value"], data.rotate.value);
            if (r.contains("velocity") && r["velocity"].is_array()) data.rotate.velocity = JsonToVec3(r["velocity"], data.rotate.velocity);
            if (r.contains("acceleration") && r["acceleration"].is_array()) data.rotate.acceleration = JsonToVec3(r["acceleration"], data.rotate.acceleration);

            // random ranges
            if (r.contains("randomRange_value") && r["randomRange_value"].is_object())
            {
                const auto& rr = r["randomRange_value"];
                if (rr.contains("min") && rr["min"].is_array()) data.rotate.randomRange_value.min = JsonToVec3(rr["min"], data.rotate.randomRange_value.min);
                if (rr.contains("max") && rr["max"].is_array()) data.rotate.randomRange_value.max = JsonToVec3(rr["max"], data.rotate.randomRange_value.max);
            }
            if (r.contains("randomRange_velocity") && r["randomRange_velocity"].is_object())
            {
                const auto& rr = r["randomRange_velocity"];
                if (rr.contains("min") && rr["min"].is_array()) data.rotate.randomRange_velocity.min = JsonToVec3(rr["min"], data.rotate.randomRange_velocity.min);
                if (rr.contains("max") && rr["max"].is_array()) data.rotate.randomRange_velocity.max = JsonToVec3(rr["max"], data.rotate.randomRange_velocity.max);
            }
            if (r.contains("randomRange_acceleration") && r["randomRange_acceleration"].is_object())
            {
                const auto& rr = r["randomRange_acceleration"];
                if (rr.contains("min") && rr["min"].is_array()) data.rotate.randomRange_acceleration.min = JsonToVec3(rr["min"], data.rotate.randomRange_acceleration.min);
                if (rr.contains("max") && rr["max"].is_array()) data.rotate.randomRange_acceleration.max = JsonToVec3(rr["max"], data.rotate.randomRange_acceleration.max);
            }
        }

        // translate
        if (j.contains("translate") && j["translate"].is_object())
        {
            const auto& t = j["translate"];
            // flags
            if (t.contains("isRandom_value")) data.translate.isRandom_value = t["isRandom_value"].get<bool>();
            if (t.contains("isRandom_velocity")) data.translate.isRandom_velocity = t["isRandom_velocity"].get<bool>();
            if (t.contains("isRandom_acceleration")) data.translate.isRandom_acceleration = t["isRandom_acceleration"].get<bool>();

            // values
            if (t.contains("value") && t["value"].is_array()) data.translate.value = JsonToVec3(t["value"], data.translate.value);
            if (t.contains("velocity") && t["velocity"].is_array()) data.translate.velocity = JsonToVec3(t["velocity"], data.translate.velocity);
            if (t.contains("acceleration") && t["acceleration"].is_array()) data.translate.acceleration = JsonToVec3(t["acceleration"], data.translate.acceleration);

            // random ranges
            if (t.contains("randomRange_value") && t["randomRange_value"].is_object())
            {
                const auto& rr = t["randomRange_value"];
                if (rr.contains("min") && rr["min"].is_array()) data.translate.randomRange_value.min = JsonToVec3(rr["min"], data.translate.randomRange_value.min);
                if (rr.contains("max") && rr["max"].is_array()) data.translate.randomRange_value.max = JsonToVec3(rr["max"], data.translate.randomRange_value.max);
            }
            if (t.contains("randomRange_velocity") && t["randomRange_velocity"].is_object())
            {
                const auto& rr = t["randomRange_velocity"];
                if (rr.contains("min") && rr["min"].is_array()) data.translate.randomRange_velocity.min = JsonToVec3(rr["min"], data.translate.randomRange_velocity.min);
                if (rr.contains("max") && rr["max"].is_array()) data.translate.randomRange_velocity.max = JsonToVec3(rr["max"], data.translate.randomRange_velocity.max);
            }
            if (t.contains("randomRange_acceleration") && t["randomRange_acceleration"].is_object())
            {
                const auto& rr = t["randomRange_acceleration"];
                if (rr.contains("min") && rr["min"].is_array()) data.translate.randomRange_acceleration.min = JsonToVec3(rr["min"], data.translate.randomRange_acceleration.min);
                if (rr.contains("max") && rr["max"].is_array()) data.translate.randomRange_acceleration.max = JsonToVec3(rr["max"], data.translate.randomRange_acceleration.max);
            }
        }

        // target
        if (j.contains("target") && j["target"].is_array())
        {
            data.target = JsonToVec3(j["target"], data.target);
        }

		// spreadAngle
		data.spreadAngle = j.value("spreadAngle", data.spreadAngle);

		// useTarget
		data.useTarget = j.value("useTarget", data.useTarget);

        // speed
		data.speed = j.value("speed", data.speed);

        data.spawnDependent = j.value("spawnDependent", data.spawnDependent);

        // color
        if (j.contains("color") && j["color"].is_array())
        {
            Vector4 vc = JsonToVec4(j["color"], ConvertUintToVector4(data.color));
            data.color = ConvertVector4ToUint(vc);
        }

        // density / counts
        data.particlesPerEmission = j.value("particlesPerEmission", data.particlesPerEmission);
        data.emissionDelay = j.value("emissionDelay", data.emissionDelay);
        data.liveMax = j.value("liveMax", data.liveMax);

        // billboard
        data.isBillboard = j.value("isBillboard", data.isBillboard);

        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "LoadFromJson: exception: " << ex.what() << " path=" << filePathStr << "\n";
        return false;
    }
    catch (...)
    {
        std::cerr << "LoadFromJson: unknown exception path=" << filePathStr << "\n";
        return false;
    }
}



json JsonManager::Vec3ToJson(const Vector3& v)
{
    return json::array({ v.x, v.y, v.z });
}
Vector3 JsonManager::JsonToVec3(const json& j, const Vector3& def = { 0,0,0 })
{
    if (!j.is_array() || j.size() < 3) return def;
    return { j[0].get<float>(), j[1].get<float>(), j[2].get<float>() };
}
json JsonManager::Vec4ToJson(const Vector4& v)
{
    return json::array({ v.x, v.y, v.z, v.w });
}
Vector4 JsonManager::JsonToVec4(const json& j, const Vector4& def = { 0,0,0,0 })
{
    if (!j.is_array() || j.size() < 4) return def;
    return { j[0].get<float>(), j[1].get<float>(), j[2].get<float>(), j[3].get<float>() };
}
