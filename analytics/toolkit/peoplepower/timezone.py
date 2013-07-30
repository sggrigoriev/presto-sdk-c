'''
timezone
Created on July 1, 2013
@author: Arun Varma
'''

class Timezone:
    '''
    __init__
    @param idNo: String
    @param offset: int
    @param dst: boolean (daylight savings time)
    '''
    def __init__(self, idNo, offset = None, dst = None):
        self.id = idNo
        if offset != None:
            self.offset = offset
        if dst != None:
            self.dst = dst