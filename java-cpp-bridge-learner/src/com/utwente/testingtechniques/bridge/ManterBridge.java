package com.utwente.testingtechniques.bridge;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class ManterBridge {
    static BufferedWriter pBW;
    public static ManterModel mm;
    public static boolean programReady = false;
    static BufferedReader input;
    static BufferedReader error;

    public static class Register {
        public int port;
        public int value;

        Register(int port, int value) {
            this.port = port;
            this.value = value;
        }

        String getRegister() {
            return Integer.toString(port) + '_' + Integer.toHexString(value);
        }

        int getPort() {
            return this.port;
        }

        int getValue() {
            return this.value;
        }

        void setPort(int port) {
            this.port = port;
        }

        void setValue(int value) {
            this.value = value;
        }

        void print() {
            System.out.println("Port: " + this.port + "\n Value: " + this.value + "\n");
        }
    }

    public static class ManterModel {
        ArrayList<Register> currentRegisterState;
        String statusCommand;
        String resetCommand;
        String setCommand;
        String getCommand;
        ArrayList<Integer> filterPorts;

        ManterModel() {
            this.currentRegisterState = new ArrayList<>();
            this.filterPorts = new ArrayList<Integer>();
            this.filterPorts.add(216);
            this.filterPorts.add(215);
            this.filterPorts.add(305);
            this.filterPorts.add(300);
            statusCommand = "status2";
            resetCommand = "reset";
            getCommand = "get";
            setCommand = "set";
        }

        void writeToBW(String str) throws IOException {
            pBW.write(str + "\n");
            pBW.flush();
        }

        String getCustomMessage() throws IOException {
            String currentLine;
            while (true) {
                currentLine = input.readLine();
                if (currentLine.contains("utwente_status2")) {
                    return currentLine;
                }
            }
        }

        void parseSaveCustomMessage(String message) {
            String croppedMessage = message.substring(17, message.length() - 1);
            String[] registers = croppedMessage.split(" ");
            String[] registerAndValue;
            int register;
            for (String val : registers) {
                try {
                    registerAndValue = val.split(",");
                    register = Integer.parseInt(registerAndValue[0].replaceAll("\\s", ""));
                    Integer value = Integer.decode("0x" + registerAndValue[1].replaceAll("\\s", ""));
                    mm.setOrAddPseudoRegister(register, value);
                } catch (Exception e) {
                    System.out.println("\n\n!!!Exception message!!!\n" + message);
                    System.out.println(croppedMessage);
                    System.out.println(e.getMessage() + "\n\n");
                }
            }
        }

        public ArrayList<Register> getStatus() throws IOException {
            this.writeToBW(statusCommand);
            String message;
            message = this.getCustomMessage();
            this.parseSaveCustomMessage(message);
            return this.currentRegisterState;
        }

        public void reset() throws IOException {
            this.writeToBW(resetCommand);
        }


        public void setRegister(int p, int v) throws IOException {
            String composedStr = this.setCommand + " " + Integer.toString(p) + "_" + Integer.toHexString(v);
            this.writeToBW(composedStr);
        }

        void getRegister(int p) throws IOException {
            String composedStr = this.getCommand + " " + Integer.toString(p);
            this.writeToBW(composedStr);
        }

        void setOrAddPseudoRegister(int p, int v) {
            boolean found = false;
            for (Register register : this.currentRegisterState) {
                if (register.port == p) {
                    register.setValue(v);
                    found = true;
                }
            }
            if (!found) {
                this.currentRegisterState.add(new Register(p, v));
            }
        }

        void printCurrentState() {
            for (Register reg : this.currentRegisterState) {
                reg.print();
            }
        }

        public String getCurrentStateString() {
            StringBuilder stateToStr = new StringBuilder();
            for (Register reg : this.currentRegisterState) {
                if (this.filterPorts.contains(reg.getPort())) {
                    stateToStr.append(reg.getPort()).append("x").append(reg.getValue()).append(" ");
                }
            }
            return stateToStr.toString();
        }

        public ArrayList<Register> getRegisters() {
            return this.currentRegisterState;
        }

    }


    public static void run(String[] args) throws InterruptedException, IOException {
        if (args.length == 0) {
            System.out.println("Please provide as argument relative path to the program/command you want to execute.");
            return;
        }

        //Scanner s = new Scanner(System.in);
        List<String> command = new ArrayList<>();

        command.add(args[0]);

        ProcessBuilder builder = new ProcessBuilder(command);
        //Map<String, String> environ = builder.environment();

        final Process process = builder.start();

        InputStream iutOut;
        InputStream iutErr;


        iutOut = process.getInputStream();
        input = new BufferedReader(new InputStreamReader(iutOut));

        iutErr = process.getErrorStream();
        error = new BufferedReader(new InputStreamReader(iutErr));

        mm = new ManterModel();

        //IUTReader iutReader = new IUTReader(iutOut);
        //iutReader.start();
        //ErrReader errReader = new ErrReader(iutErr);
        //errReader.start();

        pBW = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()));

        //String commandToWrite;
        //for (; ; ) {
        //    try {
        //        commandToWrite = s.nextLine();
        //        if (commandToWrite.equals("quit")) {
        //            break;
        //        }
        //Thread.sleep(5000);
        //pBW.write("status" + "\n");
        //pBW.flush();
        //    } catch (Exception e) {
        //        System.out.println(e.getMessage());
        //    }
        //}
        //pBW.close();
        //process.destroy();
    }

    public static class ErrReader extends Thread {
        private BufferedReader r;

        ErrReader(InputStream is) {
            this.r = new BufferedReader(new InputStreamReader(is));
        }

        public void run() {
            System.out.println("ERRReader running!");
            String s;
            while (true) {
                try {
                    s = r.readLine();
                    if (s == null) {
                        System.err.println("errReader read eof");
                        return;
                    }
//                    TODO: commented out
//                    System.err.println("err: " + s);
                } catch (Exception e) {
                    System.err.println("iutReader got exception: " + e.getMessage());
                }
            }
        }
    }

    public static class IUTReader extends Thread {
        private BufferedReader r;

        IUTReader(InputStream is) {
            this.r = new BufferedReader(new InputStreamReader(is));
        }

        public void run() {
            System.out.println("IUTReader running!");
            String b;
            String[] arr;
            String[] arrReg;
            int register;
            int value;
            while (true) {
                try {
                    b = r.readLine();
                    if (b == null) {
                        break;
                    }
                    if (b.contains("utwente_status2")) {
                        b = b.substring(17, b.length() - 1);
                        arrReg = b.split(" ");
                        for (String val : arrReg) {
                            arr = val.split(",");
                            register = Integer.parseInt(arr[0].replaceAll("\\s", ""));
                            value = Integer.decode("0x" + arr[1].replaceAll("\\s", ""));
                            mm.setOrAddPseudoRegister(register, value);
                        }
                    }
                } catch (EOFException e) {
                    System.err.println("iutReader got eof exception: " + e.getMessage());
                    return;
                } catch (Exception e) {
                    System.err.println("iutReader got exception: " + e.getMessage());
                }
            }
        }
    }
}
