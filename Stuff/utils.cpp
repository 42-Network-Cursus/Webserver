// Handle chuncked data when using send()
int sendAll(int s, char *buf, int *len) {
	int total = 0; // Bytes sent
	int bytesLeft = *len; // Bytes left to send
	int n;

	while (total < *len) {
		n = send(s, buf + total, bytesLeft, 0);
		if (n == -1)
			break;
		total += n;
		bytesLeft -= n;
	}

	*len = total; 

	return n == -1 ? -1 : 0;
}