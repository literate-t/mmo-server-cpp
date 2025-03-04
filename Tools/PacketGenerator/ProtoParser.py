
class ProtoParser():
    def __init__(self, start_id, recv_prefix, send_prefix) -> None:
        self.recv_packet = []   # recv packet list
        self.send_packet = []   # send packet list
        self.total_packet = []  # total packet list
        self.start_id = start_id
        self.id = start_id
        self.recv_prefix = recv_prefix
        self.send_prefix = send_prefix
        
    def parse_proto(self, path):
        f = open(path, 'r')
        lines = f.readlines()
        
        for line in lines:
            if False == line.startswith("message"):
                continue
            # split 함수에 아무 것도 넣지 않으면 공백 문자 기준으로 자른다(\\n, \\r, \\t, \\f)
            # message 뒤에 등장하는 것이 패킷의 이름이 된다
            packet_name = line.split()[1].upper()
            # 수신 패킷은 recv_packet 멤버변수에 넣고
            if packet_name.startswith(self.recv_prefix):
                self.recv_packet.append(Packet(packet_name, self.id))
            # 송신 패킷은 send_packet 멤버변수에 넣는다
            elif packet_name.startswith(self.send_prefix):
                self.send_packet.append(Packet(packet_name, self.id))
            else:
                continue
            
            # 패킷 아이디를 부여하려면 전부 알아야 하므로 순차적으로 넣는다
            self.total_packet.append(Packet(packet_name, self.id))
            self.id += 1
        
        f.close()
        
class Packet:
    def __init__(self, name, id):
        self.name = name
        self.id = id