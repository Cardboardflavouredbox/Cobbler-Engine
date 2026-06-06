import bpy


def write_some_data(context, filepath):
    print("running write_some_data...")
    f = open(filepath, "w", encoding='utf-8')
    
    
    selected_objects = bpy.context.scene.objects
    
    for obj in selected_objects:
        if obj and obj.type == 'MESH':
            # Get the global transformation matrix
            matrix_world = obj.matrix_world
            
            active_uv_layer = obj.data.uv_layers.active.data
        
            f.write(f"O {obj.name}\n")
            
            for s in obj.material_slots:
                if s.material and s.material.use_nodes:
                    for n in s.material.node_tree.nodes:
                        if n.type == 'TEX_IMAGE':
                            f.write(f"T {n.image.name}\n")
                    
            for vert in obj.data.vertices:
                # Convert local coordinate to world coordinate
                world_coord = matrix_world @ vert.co
            
                # Format and save data: Index, X, Y, Z
                f.write(f"P {world_coord.x:f} {world_coord.y:f} {world_coord.z:f}\n")
            for face in obj.data.polygons:
                # Convert local coordinate to world coordinate
                vertices = list(face.vertices)
                normal = face.normal
            
                # Format and save data: Index, X, Y, Z
                f.write(f"F {vertices[0]} {vertices[1]} {vertices[2]}/{normal.x:f} {normal.y:f} {normal.z:f}")
                
                for loop_idx in face.loop_indices:
                    uv_loop = active_uv_layer[loop_idx]
                    f.write(f"/{uv_loop.uv[0]:f} {uv_loop.uv[1]:f}")
                f.write(f"\n")
    
    
    f.close()

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """Cobbler Engine thing"""
    bl_idname = "export_test.some_data"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Some Data"

    # ExportHelper mix-in class uses this.
    filename_ext = ".cbm"

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
        return write_some_data(context, self.filepath)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Cobbler Engine .cbm file")


# Register and add to the "file selector" menu (required to use F3 search "Text Export Operator" for quick access).
def register():
    bpy.utils.register_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export_test.some_data('INVOKE_DEFAULT')
