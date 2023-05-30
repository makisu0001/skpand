import json
import os
import sys
import bpy

# 获取命令行参数
args = sys.argv[sys.argv.index("--") + 1 :]

filepath = args[0]
if not filepath:
    print("请传入skp文件路径")

# 解析文件名
filename = os.path.basename(filepath)
filename_no_ext = os.path.splitext(filename)[0]

# 设置输出路径和文件名
script_dir = os.path.dirname(os.path.abspath(__file__))
output_path = f"{script_dir}/output/"

addon_name = "SketchUp Importer"  # 将其替换为您要启用的插件名称

# 获取偏好设置
preferences = bpy.context.preferences

# 获取插件偏好设置
addon_prefs = preferences.addons.get(addon_name)

# 如果插件已安装但未启用，则启用插件
if addon_prefs and not addon_prefs.enabled:
    addon_prefs.enabled = True
    print(f"已启用插件：{addon_name}")
else:
    print(f"插件未找到或已启用：{addon_name}")

# 加载 .skp 文件
bpy.ops.import_scene.skp(filepath=filepath)

# 设置渲染分辨率
bpy.context.scene.render.resolution_x = 800
bpy.context.scene.render.resolution_y = 600

# 获取场景中的所有对象
all_objects = bpy.context.scene.objects

# 读取 JSON 文件
with open(f"{output_path}{filename_no_ext}/scenes.json", "r") as file:
    scenes = json.load(file)

# 筛选出相机对象
cameras = [obj for obj in all_objects if obj.type == "CAMERA"]
print(f"相机数量：{len(cameras)}")
for index, camera in enumerate(cameras):
    # 获取原相机的参数
    camera_name = camera.data.name
    location = camera.location.copy()
    rotation_euler = camera.rotation_euler.copy()
    lens = camera.data.lens

    # if scene[""]:

    # 删除原相机
    bpy.data.objects.remove(camera, do_unlink=True)

    # 创建一个新的透视相机
    name = camera_name.split(":")[1].strip()
    persp_camera_data = bpy.data.cameras.new(name=name)
    scene = scenes[name]
    ortho = scene["ortho"]
    if ortho == True:
        persp_camera_data.type = "ORTHO"
        ortho_scale = scene["scale"]
    else:
        persp_camera_data.type = "PERSP"

    persp_camera = bpy.data.objects.new(name=name, object_data=persp_camera_data)
    bpy.context.scene.collection.objects.link(persp_camera)

    # 设置新相机的参数
    persp_camera.location = location
    persp_camera.rotation_euler = rotation_euler

    # 切换相机
    bpy.context.scene.camera = persp_camera

    # 打印相机对象的名称
    output_filename = f"{filename_no_ext}/facade2_{index}.png"

    # 设置渲染设置
    bpy.context.scene.render.image_settings.file_format = "PNG"
    bpy.context.scene.render.filepath = f"{output_path}{filename_no_ext}/{name}.png"

    # 执行渲染
    bpy.ops.render.render(write_still=True)
