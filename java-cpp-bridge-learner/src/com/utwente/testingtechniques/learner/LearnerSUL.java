package com.utwente.testingtechniques.learner;

import com.utwente.testingtechniques.bridge.ManterBridge;
import de.learnlib.api.SUL;
import de.learnlib.api.SULException;

import java.io.IOException;
import java.util.ArrayList;

/**
 * Example of a three-state system, hard-coded.
 *
 * @author Ramon Janssen
 */
public class LearnerSUL implements SUL<String, String> {
    private String currentState;
    private static boolean VERBOSE = true;

    LearnerSUL() {
        this.currentState = "not_dropping";
    }

    @Override
    public void pre() {
        //utwente_io(218, ff)
        //utwente_io(219, ff)
        //utwente_io(213, 80)
        //utwente_io(217, 80)
        //utwente_io(210, ff)
        //utwente_io(211, ff)
        //utwente_io(212, ff)
        //utwente_io(214, ff)
        //utwente_io(215, ff)
        //utwente_io(216, ff)
        //utwente_io(308, db)
        //utwente_io(309, ff)
        //utwente_io(303, 90)
        //utwente_io(307, 90)
        //utwente_io(301, ff)
        //utwente_io(302, ff)
        //utwente_io(305, ff)
        //utwente_io(306, ff)
        //utwente_io(300, ff)
        //utwente_io(304, ff)
        if (VERBOSE) {
            System.out.println("Starting SUL\n");
        }
        try {
            ManterBridge.mm.reset();
            this.currentState = "not_dropping";

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void post() {
        // add any code here that should be run at the end of every 'session'
        if (VERBOSE) {
            System.out.println("\nShutting down SUL");
        }
    }

    @Override
    public String step(String input) throws SULException {
        String previousState = this.currentState;
        String output = null;
        try {
            output = makeTransition(input);
            this.currentState = output;
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
        String nextState = this.currentState;
        if (VERBOSE) {
            System.out.println("\nPrevious State: " + previousState);
            System.out.println("Input: " + input + " | Output: " + output);
            System.out.println("Next State: " + nextState);
        }
        return output;
    }

    /**
     * The behaviour of the SUL. It takes one input, and returns an output. It now
     * contains a hardcoded state-machine (so the result is easy to check). To learn
     * an external program/system, connect this method to the SUL (e.g. via sockets
     * or stdin/stdout) and make it perform an actual input, and retrieve an actual
     * output.
     */
    private String makeTransition(String input) throws IOException, InterruptedException {
        switch (input) {
            case "bagger_empty":
                ManterBridge.mm.setRegister(300, 0xef);
                break;
            case "bagger_full":
                ManterBridge.mm.setRegister(300, 0xff);
                break;
            default:
                throw new SULException(new IllegalArgumentException("Argument '" + input + "' was not handled"));
        }

        long currTime = System.currentTimeMillis();
        int counter = 0;
        boolean notDropping = false;
        boolean startedDropping = false;
        while (currTime + 1000 > System.currentTimeMillis()) {
            counter++;
            boolean isBucketDropping = false;
            ArrayList<ManterBridge.Register> tempArrReg = ManterBridge.mm.getStatus();
            //211 && 212 == ff (No buckets open)
            for (ManterBridge.Register reg : tempArrReg) {
                if ((reg.port == 211 || reg.port == 212)) {
                    isBucketDropping |= reg.value != Integer.decode("0xff");
                }
            }
            if (!isBucketDropping && !notDropping) {
                notDropping = true;
            }
            if (notDropping && isBucketDropping) {
                startedDropping = true;
            }
        }
        System.out.println("Counter: " + counter);
        return startedDropping ? "dropping" : "not_dropping";
    }
}