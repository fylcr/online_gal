// Decompile every function in the current program to pseudo-C.
//
// Usage (from analyzeHeadless -postScript):
//   DecompileAll.java <outputDir> [maxFunctions] [nameFilterRegex]
//
// Writes:
//   <outputDir>/functions/<sanitized-name>__<address>.c   one file per function
//   <outputDir>/index.tsv                                 name, address, size, file
//   <outputDir>/summary.txt                               counts and failures
//
// One file per function rather than one blob: the callers of a given symbol are
// then greppable without loading a 100 MB text file, and a partially complete
// run still leaves finished functions on disk.
//
// The program is assumed to have been imported (and analysed) by the caller;
// this script never modifies the project.

import java.io.File;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.SourceType;

public class DecompileAll extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) {
            println("usage: DecompileAll.java <outputDir> [maxFunctions] [nameFilterRegex]");
            return;
        }
        File outDir = new File(args[0]);
        int maxFunctions = args.length > 1 ? Integer.parseInt(args[1]) : Integer.MAX_VALUE;
        String filter = args.length > 2 && !args[2].isEmpty() ? args[2] : null;

        File funcDir = new File(outDir, "functions");
        if (!funcDir.isDirectory() && !funcDir.mkdirs()) {
            println("cannot create " + funcDir);
            return;
        }

        DecompInterface decomp = new DecompInterface();
        decomp.toggleCCode(true);
        decomp.toggleSyntaxTree(true);
        if (!decomp.openProgram(currentProgram)) {
            println("decompiler failed to open program: " + decomp.getLastMessage());
            return;
        }

        List<Function> todo = new ArrayList<>();
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        while (it.hasNext()) {
            Function f = it.next();
            if (f.isThunk() || f.isExternal()) {
                continue;
            }
            if (filter != null && !f.getName().matches(filter)) {
                continue;
            }
            todo.add(f);
            if (todo.size() >= maxFunctions) {
                break;
            }
        }

        println("decompiling " + todo.size() + " functions into " + funcDir);
        PrintWriter index = new PrintWriter(new File(outDir, "index.tsv"), "UTF-8");
        index.println("name\taddress\tsize\tfile");

        int ok = 0, failed = 0, i = 0;
        for (Function f : todo) {
            i++;
            if (monitor.isCancelled()) {
                println("cancelled after " + i + " functions");
                break;
            }
            String name = f.getName();
            String addr = f.getEntryPoint().toString();
            String file = sanitize(name) + "__" + addr.replace(':', '_') + ".c";
            String pseudo;
            try {
                DecompileResults res = decomp.decompileFunction(f, 60, monitor);
                if (res != null && res.decompileCompleted()) {
                    pseudo = res.getDecompiledFunction().getC();
                } else {
                    pseudo = "/* decompilation failed: "
                            + (res == null ? "no result" : res.getErrorMessage()) + " */\n";
                    failed++;
                }
            } catch (Exception e) {
                pseudo = "/* decompilation threw: " + e + " */\n";
                failed++;
            }
            if (!pseudo.startsWith("/* decompilation failed")
                    && !pseudo.startsWith("/* decompilation threw")) {
                ok++;
            }

            // Provenance header so a reader can map a file back to the binary and
            // tell auto-generated names from real exported symbols.
            StringBuilder sb = new StringBuilder();
            sb.append("// ").append(name).append('\n');
            sb.append("// address: ").append(addr).append('\n');
            sb.append("// size: ").append(f.getBody().getNumAddresses()).append(" bytes\n");
            SourceType st = f.getSymbol().getSource();
            sb.append("// symbol source: ").append(st).append('\n');
            if (f.getSignature() != null) {
                sb.append("// signature: ").append(f.getSignature().getPrototypeString()).append('\n');
            }
            sb.append('\n').append(pseudo);
            if (!pseudo.endsWith("\n")) {
                sb.append('\n');
            }

            PrintWriter w = new PrintWriter(new File(funcDir, file), "UTF-8");
            w.print(sb);
            w.close();
            index.println(name + "\t" + addr + "\t" + f.getBody().getNumAddresses() + "\tfunctions/" + file);

            if (i % 200 == 0) {
                println("  " + i + "/" + todo.size() + " (" + ok + " ok, " + failed + " failed)");
            }
        }
        index.close();
        decomp.dispose();

        PrintWriter sum = new PrintWriter(new File(outDir, "summary.txt"), "UTF-8");
        sum.println("program: " + currentProgram.getName());
        sum.println("executable: " + currentProgram.getExecutablePath());
        sum.println("language: " + currentProgram.getLanguageID());
        sum.println("compiler: " + currentProgram.getCompilerSpec().getCompilerSpecID());
        sum.println("imageBase: " + currentProgram.getImageBase());
        sum.println("functions decompiled: " + ok);
        sum.println("functions failed: " + failed);
        sum.println("functions attempted: " + (ok + failed));
        sum.close();

        println("done: " + ok + " ok, " + failed + " failed");
    }

    private static String sanitize(String name) {
        StringBuilder sb = new StringBuilder(name.length());
        for (int i = 0; i < name.length(); i++) {
            char c = name.charAt(i);
            if (Character.isLetterOrDigit(c) || c == '_' || c == '-' || c == '.') {
                sb.append(c);
            } else {
                sb.append('_');
            }
        }
        String s = sb.toString();
        return s.length() > 150 ? s.substring(0, 150) : s;
    }
}
