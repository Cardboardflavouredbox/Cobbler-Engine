import bpy
import json


def write_some_data(context, filepath, use_some_setting):
    print("running write_some_data...")
    
    
    Map = bpy.data.objects.get("Map")
    
    vertdict = {}
    
    facelist = []
    
    for child in Map.children:
      if child and child.type == 'MESH':
        if child.mode != 'OBJECT':
            bpy.ops.object.mode_set(mode='OBJECT')
            
        mesh = child.data
        
        for vert in mesh.vertices:
            vertdict[vert.index] = json.dumps(list(child.matrix_world @ vert.co))
            
        for face in mesh.polygons:
            tempface = {
                 "doublesided": True,
                 "texture": "Wall",
                 "xloop": 1,
                 "yloop": 1,
                 "points": json.dumps(list(face.vertices)),
                 "UVs": json.dumps([
                    [
                       0,
                       0
                    ],
                    [
                       1,
                       0
                    ],
                    [
                       1,
                       1
                    ],
                    [
                       0,
                       1
                    ]
                 ]),
                 "shade": json.dumps([
                    255,
                    255,
                    255,
                    255
                 ])
              }
            facelist.append(tempface)
            
    
    f = open(filepath, "w", encoding='utf-8')
    
    
    data = {
    "Points": json.dumps(vertdict),
    "mapfaces": json.dumps(facelist),
    "Entities":json.dumps([
    ]),
    "props":json.dumps([

    ]),
    "skybox": "Sky"
    }

    
    
    json.dump(data,f)
    
    f.close()

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export_test.some_data"  # Important since its how bpy.ops.import_test.some_data is constructed.
    bl_label = "Export Some Data"

    # ExportHelper mix-in class uses this.
    filename_ext = ".json"

    filter_glob: StringProperty(
        default="*.txt",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    use_setting: BoolProperty(
        name="Example Boolean",
        description="Example Tooltip",
        default=True,
    )

    type: EnumProperty(
        name="Example Enum",
        description="Choose between two items",
        items=(
            ('OPT_A', "First Option", "Description one"),
            ('OPT_B', "Second Option", "Description two"),
        ),
        default='OPT_A',
    )

    def execute(self, context):
        return write_some_data(context, self.filepath, self.use_setting)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Text Export Operator")


# Register and add to the "file selector" menu (required to use F3 search "Text Export Operator" for quick access).
def register():
    bpy.utils.register_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # Test call.
    bpy.ops.export_test.some_data('INVOKE_DEFAULT')
