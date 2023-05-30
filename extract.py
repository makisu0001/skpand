import json
import os

data = {}

for s in scenes:
    try:
        aspect_ratio = getattr(s.camera, "aspect_ratio", None)
    except:
        aspect_ratio = None
    try:
        fov = getattr(s.camera, "fov", None)
    except:
        fov = None
    try:
        ortho = getattr(s.camera, "ortho", None)
    except:
        ortho = None
    try:
        perspective = getattr(s.camera, "perspective", None)
    except:
        perspective = None
    try:
        scale = getattr(s.camera, "scale")
    except:
        scale = None
    data[getattr(s, "name", None)] = {
        "aspect_ratio": aspect_ratio,
        "fov": fov,
        "ortho": ortho,
        "perspective": perspective,
        "scale": scale,
    }

# for m in materials:
#     material = {
#         "name": m.name,
#         "color": m.color,
#         "texture": m.texture,
#         "opacity": m.opacity,
#     }
#     print(dir(m.texture))
#     m.texture.to_bytes(m.texture.bit_length)
#     data["materials"].append(material)

file_path = f"./output/{os.path.splitext(filename)[0]}/scenes.json"

with open(file_path, "w") as json_file:
    json.dump(data, json_file, ensure_ascii=False)
