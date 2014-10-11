
import math
import random
from RegionsManager import Expert
from SimSystem import SimpleFunction as Robot


if __name__ == "__main__":

    # number of time step
    sim_duration = 5000

    # instantiate an Expert
    expert = Expert()

    # instantiate a Robot
    robot = Robot()

    # initial conditions
    t = 0
    S = (50,)
    M = (60,)

    while t < sim_duration:
        t += 1

        print("\nTest case ", S, M)

        # have the expert make prediction
        S1_predicted = expert.predict(S, M)
        print("Predicted S1: ", S1_predicted)

        # do action
        robot.actuate(M)

        # read sensor
        S1 = robot.report()

        # add exemplar to expert
        expert.append(S + M, S1, S1_predicted)

        expert.split()  # won't actually split if the condition is not met


        # random action or the best action
        dice = random.random()
        if dice < 0.2:
            M1 = (random.randrange(-100, 100),)
        else:
            M1, L = expert.get_next_action(S1)
            print("Expected Reward", L)
        print("Next Action", M1)

        # set to current state
        S = S1
        M = M1

    expert.print()

