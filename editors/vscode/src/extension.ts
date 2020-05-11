"use strict";

import * as path from "path";
import * as os from 'os';
import { workspace, ExtensionContext } from "vscode";
import
{
    LanguageClient,
    LanguageClientOptions,
    SettingMonitor,
    ServerOptions,
    TransportKind,
    InitializeParams,
    Executable,
    ExecutableOptions
} from "vscode-languageclient";
import { Trace } from "vscode-jsonrpc";


export function activate(context: ExtensionContext)
{
    let serverExe = 'C:\\WorkingFolder\\GitHub\\fel\\build\\Debug\\fel.exe';

    // let serverCommand = context.asAbsolutePath(path.join('server', 'SampleServer.exe'));
	// let commandOptions: ExecutableOptions = { stdio: 'pipe', detached: false };
    
    let run_command = { command: serverExe, args: ['--lsp'] };
    let serverOptions: ServerOptions =
    {
        run: run_command,
        debug: run_command
    };

    let clientOptions: LanguageClientOptions =
    {
        documentSelector: [
            {
                pattern: "**/*.fel"
            }
        ],
        progressOnInitialization: true,
        synchronize: {
            // Synchronize the setting section 'languageServerExample' to the server
            configurationSection: "languageServerExample",
            fileEvents: workspace.createFileSystemWatcher("**/*.cs")
        }
    };

    // Create the language client and start the client.
    const client = new LanguageClient
    (
        "languageServerFel",
        "Fel Language Server",
        serverOptions,
        clientOptions
    );
    client.registerProposedFeatures();
    client.trace = Trace.Verbose;
    let disposable = client.start();

    context.subscriptions.push(disposable);
}
