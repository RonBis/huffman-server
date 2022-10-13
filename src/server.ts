import express from "express";
import { platform } from "os";
import { spawn } from "child_process";

const app = express();
app.use(express.json());
const port = process.env.PORT || 8080;

function getcode(msg: string, res: express.Response) {
  let proc_name = platform() == "win32" ? "/a.exe" : "/a.out"
  const proc = spawn(__dirname + proc_name, [msg]);
  proc.on("error", (e) => console.log(e));

  proc.stdout.on("data", (data) => {
    let codes = data.toString();

    if (platform() == "win32") {
      // CRLF shit
      codes = codes.split("\r\n");
    } else {
      codes = codes.split("\n");
    }
    codes.pop();

    let code_table = "";
    codes.forEach((code: string) => {
      let row = code.split(" ");
      code_table =
        code_table +
        `<tr>
          <td>${row[0]}</td>
          <td>${row[1]}</td>
          <td>${row[2]}</td>
        </tr>`;
    });

    let html = `<html>
      <head>
        <title>Huffman code</title>
        <style>
          html {
            font-family: sans-serif;
          }
          table {
            border-collapse: collapse;
            margin: 25px 0;
            font-size: 0.9em;
            min-width: 400px;
            box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);
          }
          thead tr {
            background-color: #009879;
            color: #ffffff;
            text-align: left;
          }
          table th,
          table td {
            padding: 12px 15px;
          }
          table tbody tr {
            border-bottom: 1px solid #dddddd;
          }
          table tbody tr:nth-of-type(even) {
            background-color: #f3f3f3;
          }
          table tbody tr:last-of-type {
            border-bottom: 2px solid #009879;
          }
        </style>
      </head>
      <body>
        <p>Message is <b><i>${msg}</i></b></p>
        <table>
          <caption>Huffman Codes</caption>
          <thead>
            <tr>
              <th>Character</th>
              <th>Frequency</th>
              <th>Code</th>
            </tr>
          </thead>
          <tbody>
            ${code_table}
          </tbody>
        </table>
      </body>
    </html>
    `;

    res.send(html);
  });
}

app.get("/getcode", (req, res) => {
  getcode(req.query.msg as any, res);
});

app.listen(port, () => console.log("server listening on port " + port));
