import bpy


def write_some_data(context, filepath):
    print("running write_some_data...")
    f = open(filepath, "w", encoding='utf-8')
    
    scene = bpy.context.scene
    selected_objects = scene.objects
    
    
    for obj in selected_objects:
        if obj:
            if obj.type == 'ARMATURE':
                bones = obj.data.bones
                
                for bone in bones:
                    head = obj.matrix_world @ bone.head_local
                    tail = obj.matrix_world @ bone.tail_local
                    parentname = "null"
                    if bone.parent: 
                        parentname = bone.parent.name
                    z_axis = bone.z_axis
                        
                    f.write(f"SB {bone.name} {z_axis[0]:f} {z_axis[1]:f} {z_axis[2]:f}/{head[0]:f} {head[1]:f} {head[2]:f}/{tail[0]:f} {tail[1]:f} {tail[2]:f} {parentname}\n")
                
                bpy.ops.object.mode_set(mode='EDIT')
                edit_bones = obj.data.edit_bones
                
                for bone in edit_bones:
                    f.write(f"EB {bone.name} {bone.roll:f}\n")
                
                for frame in range(scene.frame_start, scene.frame_end + 1):
                    scene.frame_set(frame)  # Update scene evaluation for the frame
    
                    for p_bone in obj.pose.bones:
                        f.write(f"PB {p_bone.name} {frame}/{p_bone.location[0]:f} {p_bone.location[1]:f} {p_bone.location[2]:f}/{p_bone.scale[0]:f} {p_bone.scale[1]:f} {p_bone.scale[2]:f}/{p_bone.rotation_quaternion[0]:f} {p_bone.rotation_quaternion[1]:f} {p_bone.rotation_quaternion[2]:f} {p_bone.rotation_quaternion[3]:f}\n")
                
            elif obj.type == 'MESH':
                # Get the global transformation matrix
                matrix_world = obj.matrix_world
            
                active_uv_layer = obj.data.uv_layers.active.data
                
                vgroup_names = {g.index: g.name for g in obj.vertex_groups}
        
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
                    f.write(f"P {world_coord[0]:f} {world_coord[1]:f} {world_coord[2]:f}")
                    for g in vert.groups:
                        group_name = vgroup_names.get(g.group, "Unknown Group")
                        if g.weight > 0: f.write(f" {group_name} {g.weight:.4f}")
                    f.write(f"\n")
                for face in obj.data.polygons:
                    # Convert local coordinate to world coordinate
                    vertices = list(face.vertices)
                    normal = face.normal
                
                    # Format and save data: Index, X, Y, Z
                    f.write(f"F {vertices[0]} {vertices[1]} {vertices[2]}/{normal[0]:f} {normal[1]:f} {normal[2]:f}")
                    
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
