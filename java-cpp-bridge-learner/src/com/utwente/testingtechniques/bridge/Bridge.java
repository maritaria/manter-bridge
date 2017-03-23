package com.utwente.testingtechniques.bridge;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Scanner;

import static java.lang.Thread.sleep;

public class Bridge {

    public static void main(String[] args) throws InterruptedException, IOException {
        Scanner s = new Scanner(System.in);
        List<String> command = new ArrayList<String>();

        command.add(args[0]);

        ProcessBuilder builder = new ProcessBuilder(command);
        Map<String, String> environ = builder.environment();

        final Process process = builder.start();

        InputStream iutOut = process.getInputStream();
        InputStream iutErr = process.getErrorStream();

        IUTReader iutReader = new IUTReader(iutOut);
        iutReader.start();

        ErrReader errReader = new ErrReader(iutErr);
        errReader.start();

//        DataOutputStream asd = new DataOutputStream(process.getOutputStream());
        BufferedWriter pdos = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()));
        String commandToWrite;
//        iutReader.join();
        for (; ; ) {
            commandToWrite = s.nextLine();
            if (commandToWrite.equals("quit")) {
                break;
            }
            pdos.write("status" + "\n");
            pdos.flush();
        }

//        pdos.close();
//        process.destroy();
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
                    System.err.println("err: " + s);
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
            while (true) {
                try {
                    b = r.readLine();
                    if (b == null) {
                        break;
                    }
                    System.out.println(b);
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
