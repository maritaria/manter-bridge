package com.utwente.testingtechniques.bridge;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Scanner;

public class ManterBridge {
    private static BufferedWriter pBW;
    private static IUTReader iutReader;
    private static ErrReader errReader;
    private static ManterModel mm;

    class Register {
        int port;
        int value;

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
            System.out.println(this.port + "_" + this.value);
        }
    }

    public static class ManterModel {
        ArrayList<Register> currentRegisterState;
        String statusCommand;
        String resetCommand;
        String setCommand;
        String getCommand;

        //Commands:
        //status
        //reset
        //get PORT_VALUE
        //set PORT_VALUE
        //and more but not yet needed.

        ManterModel() {
            this.currentRegisterState = new ArrayList<Register>();
            statusCommand = "status";
            resetCommand = "reset";
            getCommand = "get";
            setCommand = "set";
        }

        void writeToBW(String str) throws IOException {
            pBW.write(str);
            pBW.flush();
        }

        public void getStatus() throws IOException {
            this.writeToBW(statusCommand);
        }

        public void reset() throws IOException {
            this.writeToBW(resetCommand);
        }


        void setRegister(int p, int v) throws IOException {
            String composedStr = this.setCommand + " " + Integer.toString(p) + "_" + Integer.toHexString(v);
            this.writeToBW(composedStr);
        }

        public void getRegister(int p) throws IOException {
            String composedStr = this.getCommand + " " + Integer.toString(p);
            this.writeToBW(composedStr);
        }

        void setPseudoRegister(int p, int v) {
            for (Register register : this.currentRegisterState) {
                if (register.getValue() == p) {
                    register.setValue(v);
                }
            }
        }

        public void printCurrentState() {
            for (Register reg : this.currentRegisterState) {
                reg.print();
            }
        }

        public ArrayList<Register> getRegisters() {
            return this.currentRegisterState;
        }

    }

    public static void main(String[] args) throws InterruptedException, IOException {
        if (args.length == 0) {
            System.out.println("Please provide as argument relative path to the program/command you want to execute.");
            return;
        }

        Scanner s = new Scanner(System.in);
        List<String> command = new ArrayList<String>();

        command.add(args[0]);

        ProcessBuilder builder = new ProcessBuilder(command);
        Map<String, String> environ = builder.environment();

        final Process process = builder.start();
        InputStream iutOut = process.getInputStream();
        InputStream iutErr = process.getErrorStream();

        mm = new ManterModel();

        iutReader = new IUTReader(iutOut);
        iutReader.start();

        errReader = new ErrReader(iutErr);
        errReader.start();

        pBW = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()));

        String commandToWrite;
        for (; ; ) {
            try {
                commandToWrite = s.nextLine();
                if (commandToWrite.equals("quit")) {
                    break;
                }
                pBW.write(commandToWrite + "\n");
                pBW.flush();
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        }
        pBW.close();
        process.destroy();
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
            String[] arr = {};
            int register;
            int value;
            while (true) {
                try {
                    b = r.readLine();
                    if (b == null) {
                        break;
                    }
                    if (b.contains("utwente_io")) {
                        b = b.substring(11, b.length() - 1);
                        arr = b.split(",");
                        register = Integer.parseInt(arr[0].replaceAll("\\s", ""));
                        value = Integer.parseInt(arr[1].replaceAll("\\s", ""));

//                        System.out.println("Register: " + arr[0].replaceAll("\\s", ""));
//                        System.out.println("Value: " + arr[1].replaceAll("\\s", ""));

                        mm.setPseudoRegister(register, value);

                        mm.printCurrentState();
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
