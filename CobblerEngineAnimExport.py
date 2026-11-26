import bpy
import os

def actionset(action):
    for ob in bpy.data.objects:
        if not ob.animation_data:
            continue

        # Set the active action
        ob.animation_data.action = action

def actionthing(action, filepath):

    dir = os.path.dirname(filepath)

    if action and action.layers:
        actionset(action)
        first_layer = action.layers[0]
        first_strip = first_layer.strips[0]

        start_frame = action.frame_range[0]
        end_frame = action.frame_range[1]


        f = open(dir+ "/" + action.name + ".anim", "w", encoding="utf-8")

        f.write(f"A {action.name} {start_frame:.0f} {end_frame:.0f}\n")

        # Extract keyframe points for each F-Curve (e.g., location, rotation, scale channels)

        for slot in action.slots:
            # Retrieve the specific channel bag for the slot
            channel_bag = first_strip.channelbag(slot)

            if channel_bag and channel_bag.fcurves:
                for fcurve in channel_bag.fcurves:
                    if fcurve.data_path == "hide_viewport":
                        templist = slot.users()

                        for i in templist:
                            f.write(
                                f"FCV {bpy.path.display_name_from_filepath(i.override_library.reference.library.filepath)}/{i.override_library.reference.name}\n"
                            )
                    else:
                        f.write(f"FC {fcurve.data_path} {fcurve.array_index}\n")
                    for keyframe in fcurve.keyframe_points:
                        frame_number = keyframe.co[0]
                        curve_value = keyframe.co[1]
                        f.write(f" {frame_number:.0f}/{curve_value:f}")
                    f.write(f"\n")
        f.close()





def write_some_data(context, filepath):
    print("running write_some_data...")

    for action in bpy.data.actions:
        if action.library is None:
            actionthing(action, filepath)

    return {"FINISHED"}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """Cobbler Engine anim"""

    bl_idname = "export_test.some_data"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Some Data"

    # ExportHelper mix-in class uses this.
    filename_ext = ".cbm"

    filter_glob: StringProperty(
        default="*.txt",
        options={"HIDDEN"},
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
            ("OPT_A", "First Option", "Description one"),
            ("OPT_B", "Second Option", "Description two"),
        ),
        default="OPT_A",
    )

    def execute(self, context):
        return write_some_data(context, self.filepath)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Cobbler Engine .anim file")


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
    bpy.ops.export_test.some_data("INVOKE_DEFAULT")
