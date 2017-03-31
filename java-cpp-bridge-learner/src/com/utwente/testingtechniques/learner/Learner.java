package com.utwente.testingtechniques.learner;

import java.io.IOException;
import java.io.Writer;
import java.util.Random;
import java.util.Scanner;

import com.utwente.testingtechniques.bridge.ManterBridge;
import net.automatalib.automata.transout.MealyMachine;
import net.automatalib.commons.dotutil.DOT;
import net.automatalib.util.graphs.dot.GraphDOT;
import net.automatalib.words.Alphabet;
import net.automatalib.words.Word;
import net.automatalib.words.impl.SimpleAlphabet;
import com.google.common.collect.ImmutableSet;
import com.google.common.collect.Lists;

import de.learnlib.algorithms.lstargeneric.ce.ObservationTableCEXHandlers;
import de.learnlib.algorithms.lstargeneric.closing.ClosingStrategies;
import de.learnlib.algorithms.lstargeneric.mealy.ExtensibleLStarMealy;
import de.learnlib.api.EquivalenceOracle;
import de.learnlib.api.LearningAlgorithm;
import de.learnlib.api.MembershipOracle;
import de.learnlib.api.SUL;
import de.learnlib.eqtests.basic.mealy.RandomWalkEQOracle;
import de.learnlib.experiments.Experiment.MealyExperiment;
import de.learnlib.oracles.SULOracle;

/**
 * Minimal example of using learnlib to learn the FSM/Mealy Machine of a SUL (System-Under-Learning).
 */
public class Learner {
    //*****************//
    // SUL information //
    //*****************//
    //    ImmutableSet<String> inputs = new ImmutableSet<>();
    // Defines the input alphabet.
    //private static final Alphabet<String> inputAlphabet
    //        = new SimpleAlphabet<String>(ImmutableSet.of(
    //        "300_EF",
    //        "300_FF"));
    private static final Alphabet<String> inputAlphabet
            = new SimpleAlphabet<String>(ImmutableSet.of(
            "bagger_empty",
            "bagger_full"));

    public static void main(String[] args) throws IOException, InterruptedException {

        // Define the System Under Learning.
        Scanner s = new Scanner(System.in);
        String command = "";

        ManterBridge.run(new String[]{"./run.sh"});

        while (!command.equals("ready")) {
            System.out.println("Please type \"ready\" when ready.");
            command = s.nextLine();
        }

//        ManterBridge.mm.getStatus();


        SUL<String, String> sul = new LearnerSUL();

        // Most testing/learning-algorithms want a membership-oracle instead of a SUL directly
        // in order to optimize system queries.
        MembershipOracle<String, Word<String>> sulOracle = new SULOracle<>(sul);


        // Choosing the EQ oracle
        EquivalenceOracle<MealyMachine<?, String, ?, String>, String, Word<String>> eqOracle
                = new RandomWalkEQOracle<>(
                0.05, // reset SUL w/ this probability before a step
                10000, // max steps (overall)
                true,  // reset step count after counterexample
                new Random(123456l), // make results reproducible
                sul    // system under learning
        );


        // Choosing a learning algorithm
        LearningAlgorithm<MealyMachine<?, String, ?, String>, String, Word<String>> learner
                = new ExtensibleLStarMealy<>(
                inputAlphabet, // Input Alphabet
                sulOracle,  // SUL membership oracle
                Lists.<Word<String>>newArrayList(),
                ObservationTableCEXHandlers.CLASSIC_LSTAR,
                ClosingStrategies.CLOSE_SHORTEST
        );


        // Setup of the experiment.
        MealyExperiment<String, String> experiment
                = new MealyExperiment<String, String>(
                learner, // learning algorithm
                eqOracle, // Equivalence Oracle
                inputAlphabet); // input alphabet

        // And finally run the experiment
        experiment.run();

        // report results
        System.out.println("-------------------------------------------------------");


        // Depending on whether DOT is installed on your OS, you either get a textual or graphical model output
        boolean DOT_INSTALLED = false;

        if (DOT_INSTALLED) {
            Writer w = DOT.createDotWriter(true);
            GraphDOT.write(experiment.getFinalHypothesis(), inputAlphabet, w);
            w.close();
        } else {
            GraphDOT.write(experiment.getFinalHypothesis(), inputAlphabet, System.out); // may throw IOException!
        }

    }


}