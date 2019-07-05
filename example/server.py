import socket, select

if __name__ == "__main__":

	CONNECTION_LIST = []	# list of socket clients
	RECV_BUFFER = 4096
	PORT = 5000

	server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	server_socket.bind(("0.0.0.0", PORT))
	server_socket.listen(10)

	# Add server socket to the list of readable connections
	CONNECTION_LIST.append(server_socket)

	print("Chat server started on port " + str(PORT))

	while 1:
        print('Reading...')
		# Get the list sockets which are ready to be read through select
		read_sockets, write_sockets, error_sockets = select.select(CONNECTION_LIST, [], [])

		for sock in read_sockets:
			if sock == server_socket:
				sockfd, addr = server_socket.accept()
				CONNECTION_LIST.append(sockfd)
				print("Client (%s, %s) connected" % addr)
			else:
				try:
					data = sock.recv(RECV_BUFFER)
					if data:
						print('Received: ' + data)
				except:
					broadcast_data(sock, "Client (%s, %s) is offline" % addr)
					print("Client (%s, %s) is offline" % addr)
					sock.close()
					CONNECTION_LIST.remove(sock)
					continue

	server_socket.close()
