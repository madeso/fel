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
    // todo(Gustav): restart server if settings change
    
    let ws = workspace;
    let fel_bin : string = ws == null ? "fel_ws" : ws.getConfiguration("fel.lsp").get("executable");
    let fel_log  : string = ws == null ? "fel_ws" : ws.getConfiguration("fel.lsp").get("log");

    let run_command =
    {
        command: fel_bin,
        args:
        [
            '--log',
            fel_log,
            '--lsp'
        ]
    };
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
