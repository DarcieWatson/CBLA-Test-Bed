import changePriority
from TeensyInterface import TeensyManager

behaviours_config = 2

if behaviours_config == 0:
    from InteractiveCmd import InteractiveCmd as cmd
elif behaviours_config == 1:
    from Behaviours import HardcodedBehaviours_test as cmd
elif behaviours_config == 2:
    from Behaviours import HardcodedBehaviours as cmd
else:
    from InteractiveCmd import InteractiveCmd as cmd


packet_size_in = 64
packet_size_out = 64


if __name__ == '__main__':

    # change priority of the the Python process to HIGH
    changePriority.SetPriority(changePriority.Priorities.HIGH_PRIORITY_CLASS)

    # instantiate Teensy Monitor
    teensy_manager = TeensyManager(import_config=True)

    # find all the Teensy

    print("Number of Teensy devices found: " + str(teensy_manager.get_num_teensy_thread()))


    # interactive code
    behaviours = cmd(teensy_manager._get_teensy_thread_list())
    behaviours.run()


