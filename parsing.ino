void parsing()
{
  if (Serial.available() > 0)
  {
    char incomingByte = Serial.read();
    if (getStarted)
    {
      if (incomingByte != ' ' && incomingByte != ';')
      {
        string_convert += incomingByte;
      }
      else
      {
        K[index] = string_convert.toFloat();
        string_convert = "";
        index++;
      }
    }
    if (incomingByte == '$')
    {
      getStarted = true;
      index = 0;
      string_convert = "";
    }
    if (incomingByte == ';')
    {
      getStarted = false;
      recievedFlag = true;
    }
  }
}
