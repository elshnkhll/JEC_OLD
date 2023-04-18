String POST_to_Server(String pdt){
  String c = "";
  if (client.connect(server, 80)) {
    Serial.println("Connected to server...");
    delay(100);
  
    // Construct the POST request that we'd like to issue
    client.println("POST "+script+" HTTP/1.0" ); 
    client.println("User-Agent: ESP8266/1.0");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.println("Connection: close");
    client.println("Content-Length: "+ String( pdt.length() ) );
    client.println("");
    client.println(pdt);
  } else {
    // If we can't establish a connection to the server:
    Serial.println("server connection failed...");
  }
  // read the response from server
  while (client.available()) {
    c = client.readStringUntil('\n');
    if(c.length() == 1){
      c = client.readStringUntil('^');
      // Serial.println(c);
    }
  }
  // close the connection.
  if (client.connected()) {
    Serial.println("Disconnecting...");
    client.stop();
  }
  return c;
}